// Copyright 1998-2015 Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;
using System.IO;

public class AndroidFacebookPlugin : ModuleRules
{
	public AndroidFacebookPlugin(TargetInfo Target)
	{
        Definitions.Add("ANDROIDFACEBOOKPLUGIN_PACKAGE=1");

        PublicIncludePaths.AddRange(
			new string[] {
				"AndroidFacebookPlugin/Public"
				// ... add public include paths required here ...
			}
			);
				
		
		PrivateIncludePaths.AddRange(
			new string[] {
				"AndroidFacebookPlugin/Private",
				// ... add other private include paths required here ...
			}
			);

        PrivateIncludePathModuleNames.AddRange(
            new string[] {
                "Settings",
                "Launch",
            }
            );

        PublicDependencyModuleNames.AddRange(
			new string[]
			{
				"Core",
                "OnlineSubsystem", 
				// ... add other public dependencies that you statically link with here ...
			}
			);
			
		
		PrivateDependencyModuleNames.AddRange(
			new string[]
			{
				"CoreUObject",
				"Engine",				
                "Json"
				// ... add private dependencies that you statically link with here ...	
			}
			);

	
        if (Target.Platform == UnrealTargetPlatform.Android)
        {
            string PluginPath = Utils.MakePathRelativeTo(ModuleDirectory, BuildConfiguration.RelativeEnginePath);
            AdditionalPropertiesForReceipt.Add(new ReceiptProperty("AndroidPlugin", Path.Combine(PluginPath, "AndroidFacebookPlugin_APL.xml")));

            PrivateIncludePaths.Add("AndroidFacebookPlugin/Private/Android");
            PrivateIncludePaths.Add("AndroidFacebookPlugin/Public/Android");

            Definitions.Add("PLATFORM_ANDROID=1");

        }
    }
}
