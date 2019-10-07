# SpatialOS Unreal GDK Example Project

<img src="https://user-images.githubusercontent.com/2433131/58086122-f28af200-7bb5-11e9-9b96-b13b747130d0.png" height="70%" width="70%">

- **LICENSE:** Use of the contents of this repository is subject to the [license](LICENSE.md)

This project requires the Online Services (available [here](https://docs.improbable.io/metagame/0.1.0/index)) in order for the party and deployment connection systems to work. Follow the [Quickstart](https://docs.improbable.io/metagame/0.1.0/content/get-started/quickstart) guide to set these up. We also recommend using a deployment pool to manage your deployments, available from [here](https://docs.improbable.io/metagame/0.1.0/content/configuration-examples/deployment-pool/overview).

We provide a custom matcher here to replace the default SampleMatcher contained in the Online Services. This exists as [Matcher.cs] in this directory, to use this matcher instead of the default then replace the `services/csharp/SampleMatcher/Matcher.cs` in the `online-services` repository.

#### Public contributors

We are not currently accepting public contributions. However, we are accepting [issues](https://github.com/spatialos/UnrealGDK/issues) and we do want your feedback.

&copy; 2019 Improbable
