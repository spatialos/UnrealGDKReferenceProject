using System;
using System.Collections.Generic;
using System.Linq;
using System.Threading;
using Grpc.Core;
using Improbable.OnlineServices.Proto.Gateway;
using Improbable.SpatialOS.Deployment.V1Alpha1;

namespace Improbable.OnlineServices.SampleMatcher
{
    public class Matcher : Improbable.OnlineServices.Base.Matcher.Matcher
    {
        private const int PlayersPerDeployment = 10;

        private const int TickMs = 200;
        private const string DefaultMatchTag = "match";
        private readonly string _project;
        private readonly string _tag;
        private readonly string ReadyTag = "ready"; // This should be the same tag a DeploymentPool looks for.
        private readonly string PlayersTag = "players";
        private readonly string InUseTag = "in_use";

        public Matcher()
        {
            _project = Environment.GetEnvironmentVariable("SPATIAL_PROJECT");
            _tag = Environment.GetEnvironmentVariable("MATCH_TAG") ?? DefaultMatchTag;
        }

        protected override void DoMatch(GatewayInternalService.GatewayInternalServiceClient gatewayClient,
            DeploymentServiceClient deploymentServiceClient)
        {
            try
            {
                var resp = gatewayClient.PopWaitingParties(new PopWaitingPartiesRequest
                {
                    Type = _tag,
                    NumParties = 1
                });
                Console.WriteLine($"Fetched {resp.Parties.Count} from gateway");

                foreach (var party in resp.Parties)
                {
                    Console.WriteLine($"Attempting to match a retrieved party of {party.Party.MemberIds.Count} players.");

                    bool requeue = false;
                    var deployment = GetDeploymentWithTag(deploymentServiceClient, _tag);
                    if (deployment != null)
                    {
                        var assignRequest = new AssignDeploymentsRequest();
                        Console.WriteLine("Found a deployment.");
                        int numPlayers = 0;
                        foreach (string tag in deployment.Tag)
                        {
                            if (tag.StartsWith(PlayersTag))
                            {
                                int.TryParse(tag.Split("_")[1], out numPlayers);
                                Console.WriteLine($"Deployment already has {numPlayers} in.");
                                break;
                            }
                        }

                        int totalPlayers = numPlayers + party.Party.MemberIds.Count;
                        if (totalPlayers <= PlayersPerDeployment)
                        {
                            if (numPlayers > 0)
                            {
                                Console.WriteLine($"Removing tag {PlayersTag}_{numPlayers}");
                                deployment.Tag.Remove($"{PlayersTag}_{numPlayers}");
                            }

                            if (totalPlayers == PlayersPerDeployment)
                            {
                                Console.WriteLine("Deployment is full, marking as in_use");
                                MarkDeploymentAsInUse(deploymentServiceClient, deployment);
                            }
                            else
                            {
                                Console.WriteLine($"Deployment isn't full, adding tag {PlayersTag}_{totalPlayers}");
                                deployment.Tag.Add($"{PlayersTag}_{totalPlayers}");
                                var req = new UpdateDeploymentRequest { Deployment = deployment };
                                deploymentServiceClient.UpdateDeployment(req);

                            }

                            assignRequest.Assignments.Add(new Assignment
                            {
                                DeploymentId = deployment.Id,
                                DeploymentName = deployment.Name,
                                Result = Assignment.Types.Result.Matched,
                                Party = party.Party
                            });
                            gatewayClient.AssignDeployments(assignRequest);
                        }
                        else
                        {
                            Console.WriteLine("Deployment does not have space for a party of this size.");
                            requeue = true;
                        }
                    }
                    else
                    {
                        Console.WriteLine( $"Unable to find a deployment with tag {_tag} in project {_project}");
                        requeue = true;
                    }

                    if (requeue)
                    {
                        Console.WriteLine("Requeueing the party");
                        AssignPartyAsRequeued(gatewayClient, party);
                    }
                }
            }
            catch (RpcException e)
            {
                if (e.StatusCode != StatusCode.ResourceExhausted && e.StatusCode != StatusCode.Unavailable)
                {
                    throw;
                }

                /* Unable to get the requested number of parties - ignore. */
                Thread.Sleep(TickMs);
            }
        }

        protected override void DoShutdown(GatewayInternalService.GatewayInternalServiceClient gatewayClient,
            DeploymentServiceClient deploymentServiceClient)
        {
            // If a matcher maintains state, here is where you hand it back to the Gateway if necessary.
        }

        private void AssignPartyAsRequeued(GatewayInternalService.GatewayInternalServiceClient gatewayClient,
            WaitingParty party)
        {
            var assignRequest = new AssignDeploymentsRequest();
            assignRequest.Assignments.Add(new Assignment
            {
                Result = Assignment.Types.Result.Requeued,
                Party = party.Party
            });
            gatewayClient.AssignDeployments(assignRequest);
        }

        private Deployment GetDeploymentWithTag(DeploymentServiceClient deploymentServiceClient, string tag)
        {
            return deploymentServiceClient
                .ListDeployments(new ListDeploymentsRequest
                {
                    ProjectName = _project,
                    DeploymentStoppedStatusFilter = ListDeploymentsRequest.Types.DeploymentStoppedStatusFilter
                        .NotStoppedDeployments,
                    View = ViewType.Basic,
                    Filters =
                    {
                        new Filter
                        {
                            TagsPropertyFilter = new TagsPropertyFilter
                            {
                                Operator = TagsPropertyFilter.Types.Operator.Equal,
                                Tag = tag
                            }
                        },
                        new Filter
                        {
                            TagsPropertyFilter = new TagsPropertyFilter
                            {
                                Operator = TagsPropertyFilter.Types.Operator.Equal,
                                Tag = ReadyTag
                            }
                        }
                    }
                }).FirstOrDefault(d => d.Status == Deployment.Types.Status.Running);
        }

        private void MarkDeploymentAsInUse(DeploymentServiceClient dplClient, Deployment dpl)
        {
            dpl.Tag.Remove(ReadyTag);
            dpl.Tag.Add(InUseTag);
            var req = new UpdateDeploymentRequest { Deployment = dpl };
            dplClient.UpdateDeployment(req);
        }
    }
}
