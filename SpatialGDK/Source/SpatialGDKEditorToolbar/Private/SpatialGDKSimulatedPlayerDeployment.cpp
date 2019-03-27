#include "SpatialGDKSimulatedPlayerDeployment.h"

#include "DesktopPlatformModule.h"
#include "EditorDirectories.h"
#include "EditorStyleSet.h"
#include "Framework/Application/SlateApplication.h"
#include "Framework/Notifications/NotificationManager.h"
#include "SharedPointer.h"
#include "SpatialGDKEditorCloudLauncherSettings.h"
#include "SpatialGDKEditorSettings.h"
#include "Widgets/Input/SButton.h"
#include "Widgets/Input/SFilePathPicker.h"
#include "Widgets/Input/SHyperlink.h"
#include "Widgets/Input/SSpinBox.h"
#include "Widgets/Layout/SBox.h"
#include "Widgets/Layout/SExpandableArea.h"
#include "Widgets/Layout/SSeparator.h"
#include "Widgets/Layout/SUniformGridPanel.h"
#include "Widgets/Notifications/SNotificationList.h"
#include "Widgets/Text/STextBlock.h"

#include "Internationalization/Regex.h"

void SSpatialGDKSimulatedPlayerDeployment::Construct(const FArguments& InArgs)
{
	const USpatialGDKEditorSettings* SpatialGDKSettings = GetDefault<USpatialGDKEditorSettings>();
	const USpatialGDKEditorCloudLauncherSettings* SpatialGDKCloudLauncherSettings = GetMutableDefault<USpatialGDKEditorCloudLauncherSettings>();
	
	ParentWindowPtr = InArgs._ParentWindow;
	SpatialGDKEditorPtr = InArgs._SpatialGDKEditor;

	ChildSlot
		[
			SNew(SBorder)
			.HAlign(HAlign_Fill)
			.BorderImage(FEditorStyle::GetBrush("ChildWindow.Background"))
			.Padding(4.0f)
			[
				SNew(SVerticalBox)
				+ SVerticalBox::Slot()
				.FillHeight(1.0f)
				.Padding(0.0f, 6.0f, 0.0f, 0.0f)
				[
					SNew(SBorder)
					.BorderImage(FEditorStyle::GetBrush("ToolPanel.GroupBorder"))
					.Padding(4.0f)
					[
						SNew(SVerticalBox)
						+ SVerticalBox::Slot()
						.AutoHeight()
						.Padding(1.0f)
						[
							SNew(SVerticalBox)
							// Build explanation set
							+ SVerticalBox::Slot()
							.AutoHeight()
							.Padding(2.0f)
							.VAlign(VAlign_Center)
							[
								SNew(SHorizontalBox)
								+ SHorizontalBox::Slot()
								.AutoWidth()
								.HAlign(HAlign_Center)
								[
									SNew(STextBlock)
									.Text(FText::FromString(FString(TEXT("NOTE: The assembly has to be built and uploaded manually. Follow the docs "))))
								]
								+ SHorizontalBox::Slot()
								.AutoWidth()
								[
									SNew(SHyperlink)
									.Text(FText::FromString(FString(TEXT("here."))))
									.OnNavigate(this, &SSpatialGDKSimulatedPlayerDeployment::OnCloudDocumentationClicked)
								]
							]
							// Separator
							+ SVerticalBox::Slot()
							.AutoHeight()
							.Padding(2.0f)
							.VAlign(VAlign_Center)
							[
								SNew(SSeparator)
							]
							// Project 
							+ SVerticalBox::Slot()
							.AutoHeight()
							.Padding(2.0f)
							[
								SNew(SHorizontalBox)
								+ SHorizontalBox::Slot()
								.FillWidth(1.0f)
								[
									SNew(STextBlock)
									.Text(FText::FromString(FString(TEXT("Project Name"))))
									.ToolTipText(FText::FromString(FString(TEXT("The name of the SpatialOS project."))))
								]
								+ SHorizontalBox::Slot()
								.FillWidth(1.0f)
								[
									SNew(SEditableTextBox)
									.Text(FText::FromString(SpatialGDKCloudLauncherSettings->GetProjectName()))
									.ToolTipText(FText::FromString(FString(TEXT("The name of the SpatialOS project."))))
									.OnTextCommitted(this, &SSpatialGDKSimulatedPlayerDeployment::OnProjectNameCommited)
									.OnTextChanged(this, &SSpatialGDKSimulatedPlayerDeployment::OnProjectNameCommited, ETextCommit::Default)
								]
							]
							// Assembly Name 
							+ SVerticalBox::Slot()
							.AutoHeight()
							.Padding(2.0f)
							[
								SNew(SHorizontalBox)
								+ SHorizontalBox::Slot()
								.FillWidth(1.0f)
								[
									SNew(STextBlock)
									.Text(FText::FromString(FString(TEXT("Assembly Name"))))
									.ToolTipText(FText::FromString(FString(TEXT("The name of the assembly."))))
								]
								+ SHorizontalBox::Slot()
								.FillWidth(1.0f)
								[
									SNew(SEditableTextBox)
									.Text(FText::FromString(SpatialGDKCloudLauncherSettings->GetAssemblyName()))
									.ToolTipText(FText::FromString(FString(TEXT("The name of the assembly."))))
									.OnTextCommitted(this, &SSpatialGDKSimulatedPlayerDeployment::OnDeploymentAssemblyCommited)
									.OnTextChanged(this, &SSpatialGDKSimulatedPlayerDeployment::OnDeploymentAssemblyCommited, ETextCommit::Default)
								]
							]
							// Pirmary Deployment Name 
							+ SVerticalBox::Slot()
							.AutoHeight()
							.Padding(2.0f)
							[
								SNew(SHorizontalBox)
								+ SHorizontalBox::Slot()
								.FillWidth(1.0f)
								[
									SNew(STextBlock)
									.Text(FText::FromString(FString(TEXT("Deployment Name"))))
									.ToolTipText(FText::FromString(FString(TEXT("The name of the cloud deployment. Must be unique."))))
								]
								+ SHorizontalBox::Slot()
								.FillWidth(1.0f)
								[
									SNew(SEditableTextBox)
									.Text(FText::FromString(SpatialGDKCloudLauncherSettings->GetPrimaryDeploymentName()))
									.ToolTipText(FText::FromString(FString(TEXT("The name of the cloud deployment. Must be unique."))))
									.OnTextCommitted(this, &SSpatialGDKSimulatedPlayerDeployment::OnPrimaryDeploymentNameCommited)
									.OnTextChanged(this, &SSpatialGDKSimulatedPlayerDeployment::OnPrimaryDeploymentNameCommited, ETextCommit::Default)
								]
							]
							// Snapshot File + File Picker
							+ SVerticalBox::Slot()
							.AutoHeight()
							.Padding(2.0f)
							[
								SNew(SHorizontalBox)
								+ SHorizontalBox::Slot()
								.FillWidth(1.0f)
								[
									SNew(STextBlock)
									.Text(FText::FromString(FString(TEXT("Snapshot File"))))
									.ToolTipText(FText::FromString(FString(TEXT("The relative path to the snapshot file."))))
								]
								+ SHorizontalBox::Slot()
								.FillWidth(1.0f)
								[
									SNew(SFilePathPicker)
									.BrowseButtonImage(FEditorStyle::GetBrush("PropertyWindow.Button_Ellipsis"))
									.BrowseButtonStyle(FEditorStyle::Get(), "HoverHintOnly")
									.BrowseButtonToolTip(FText::FromString(FString(TEXT("Path to the snapshot file."))))
									.BrowseDirectory(SpatialGDKSettings->GetSpatialOSSnapshotFolderPath())
									.BrowseTitle(FText::FromString(FString(TEXT("File picker..."))))
									.FilePath_UObject(SpatialGDKCloudLauncherSettings, &USpatialGDKEditorCloudLauncherSettings::GetSnapshotPath)
									.FileTypeFilter(TEXT("Snapshot files (*.snapshot)|*.snapshot"))
									.OnPathPicked(this, &SSpatialGDKSimulatedPlayerDeployment::OnSnapshotPathPicked)
								]
							]
							// Primary Launch Config + File Picker
							+ SVerticalBox::Slot()
							.AutoHeight()
							.Padding(2.0f)
							[
								SNew(SHorizontalBox)
								+ SHorizontalBox::Slot()
								.FillWidth(1.0f)
								[
									SNew(STextBlock)
									.Text(FText::FromString(FString(TEXT("Launch Config File"))))
									.ToolTipText(FText::FromString(FString(TEXT("The relative path to the launch configuration file."))))
								]
								+ SHorizontalBox::Slot()
								.FillWidth(1.0f)
								[
									SNew(SFilePathPicker)
									.BrowseButtonImage(FEditorStyle::GetBrush("PropertyWindow.Button_Ellipsis"))
									.BrowseButtonStyle(FEditorStyle::Get(), "HoverHintOnly")
									.BrowseButtonToolTip(FText::FromString(FString(TEXT("Path to the launch configuration file."))))
									.BrowseDirectory(SpatialGDKSettings->GetSpatialOSDirectory())
									.BrowseTitle(FText::FromString(FString(TEXT("File picker..."))))
									.FilePath_UObject(SpatialGDKCloudLauncherSettings, &USpatialGDKEditorCloudLauncherSettings::GetPrimaryLanchConfigPath)
									.FileTypeFilter(TEXT("Launch configuration files (*.json)|*.json"))
									.OnPathPicked(this, &SSpatialGDKSimulatedPlayerDeployment::OnPrimaryLaunchConfigPathPicked)
								]
							]
							// Separator
							+ SVerticalBox::Slot()
							.AutoHeight()
							.Padding(2.0f)
							.VAlign(VAlign_Center)
							[
								SNew(SSeparator)
							]
							// Explanation text
							+ SVerticalBox::Slot()
							.AutoHeight()
							.Padding(2.0f)
							.VAlign(VAlign_Center)
							[
								SNew(SHorizontalBox)
								+ SHorizontalBox::Slot()
								.AutoWidth()
								[
									SNew(SCheckBox)
									.IsChecked(this, &SSpatialGDKSimulatedPlayerDeployment::IsSimulatedPlayersEnabled)
									.ToolTipText(FText::FromString(FString(TEXT("Toggle to scale test."))))
									.OnCheckStateChanged(this, &SSpatialGDKSimulatedPlayerDeployment::OnCheckedSimulatedPlayers)
								]
								+ SHorizontalBox::Slot()
								.FillWidth(1.0f)
								.HAlign(HAlign_Center)
								[
									SNew(STextBlock)
									.Text(FText::FromString(FString(TEXT("Simulated Player Deployment Configuration"))))
								]
							]
							// Simulated Players Deployment Name
							+ SVerticalBox::Slot()
							.AutoHeight()
							.Padding(2.0f)
							[
								SNew(SHorizontalBox)
								+ SHorizontalBox::Slot()
								.FillWidth(1.0f)
								[
									SNew(STextBlock)
									.Text(FText::FromString(FString(TEXT("Deployment Name"))))
									.ToolTipText(FText::FromString(FString(TEXT("The name of the simulated player deployment."))))
								]
								+ SHorizontalBox::Slot()
								.FillWidth(1.0f)
								[
									SNew(SEditableTextBox)
									.Text(FText::FromString(SpatialGDKCloudLauncherSettings->GetSimulatedPlayerDeploymentName()))
									.ToolTipText(FText::FromString(FString(TEXT("The name of the simulated player deployment."))))
									.OnTextCommitted(this, &SSpatialGDKSimulatedPlayerDeployment::OnSimulatedPlayerDeploymentNameCommited)
									.OnTextChanged(this, &SSpatialGDKSimulatedPlayerDeployment::OnSimulatedPlayerDeploymentNameCommited, ETextCommit::Default)
									.IsEnabled_UObject(SpatialGDKCloudLauncherSettings, &USpatialGDKEditorCloudLauncherSettings::IsSimulatedPlayersEnabled)
								]
							]
							// Simulated Players Number 
							+ SVerticalBox::Slot()
							.AutoHeight()
							.Padding(2.0f)
							[
								SNew(SHorizontalBox)
								+ SHorizontalBox::Slot()
								.FillWidth(1.0f)
								[
									SNew(STextBlock)
									.Text(FText::FromString(FString(TEXT("Number of Simulated Players"))))
									.ToolTipText(FText::FromString(FString(TEXT("The number of Simulated Players to be launch and connect to the game."))))
								]
								+ SHorizontalBox::Slot()
								.FillWidth(1.0f)
								[
									SNew(SSpinBox<uint32>)
									.ToolTipText(FText::FromString(FString(TEXT("Number of Simulated Players."))))
									.MinValue(1)
									.MaxValue(8192)
									.Value(SpatialGDKCloudLauncherSettings->GetNumberOfSimulatedPlayer())
									.OnValueChanged(this, &SSpatialGDKSimulatedPlayerDeployment::OnNumberOfSimulatedPlayersCommited)
									.IsEnabled_UObject(SpatialGDKCloudLauncherSettings, &USpatialGDKEditorCloudLauncherSettings::IsSimulatedPlayersEnabled)
								]
							]
							// Simulated Player Launch Config + File Picker
							+ SVerticalBox::Slot()
							.AutoHeight()
							.Padding(2.0f)
							[
								SNew(SHorizontalBox)
								+ SHorizontalBox::Slot()
								.FillWidth(1.0f)
								[
									SNew(STextBlock)
									.Text(FText::FromString(FString(TEXT("Simulated Player Launch Config File"))))
									.ToolTipText(FText::FromString(FString(TEXT("The relative path to the simulated player launch configuration file."))))
								]
								+ SHorizontalBox::Slot()
								.FillWidth(1.0f)
								[
									SNew(SFilePathPicker)
									.BrowseButtonImage(FEditorStyle::GetBrush("PropertyWindow.Button_Ellipsis"))
									.BrowseButtonStyle(FEditorStyle::Get(), "HoverHintOnly")
									.BrowseButtonToolTip(FText::FromString(FString(TEXT("Path to the simulated player launch configuration."))))
									.BrowseDirectory(SpatialGDKSettings->GetSpatialOSDirectory())
									.BrowseTitle(FText::FromString(FString(TEXT("File picker..."))))
									.FilePath_UObject(SpatialGDKCloudLauncherSettings, &USpatialGDKEditorCloudLauncherSettings::GetSimulatedPlayerLaunchConfigPath)
									.FileTypeFilter(TEXT("Configuration files (*.json)|*.json"))
									.OnPathPicked(this, &SSpatialGDKSimulatedPlayerDeployment::OnSimulatedPlayerLaunchConfigPathPicked)
									.IsEnabled_UObject(SpatialGDKCloudLauncherSettings, &USpatialGDKEditorCloudLauncherSettings::IsSimulatedPlayersEnabled)
								]
							]
							// Expandeable Logs
							+ SVerticalBox::Slot()
							.AutoHeight()
							.Padding(0.0f, 6.0f, 0.0f, 0.0f)
							[
								SNew(SExpandableArea)
								.AreaTitleFont(FEditorStyle::GetFontStyle(TEXT("SourceControl.LoginWindow.Font")))
								.AreaTitle(FText::FromString(FString(TEXT("Simulated Player Logs"))))
								.BorderImage(FEditorStyle::GetBrush("NoBorder"))
								.IsEnabled(true)
								.InitiallyCollapsed(false)
								.BodyContent()
								[
									SNew(SBox)
									.HeightOverride(250)
									.WidthOverride(400)
									[
										SNew(STextBlock)
										.Text(FText::FromString(FString(TEXT("Loading..."))))
									]
								]
							]
							// Buttons
							+ SVerticalBox::Slot()
							.AutoHeight()
							.VAlign(VAlign_Center)
							[
								SNew(SHorizontalBox)
								+ SHorizontalBox::Slot()
								.FillWidth(1.0f)
								.HAlign(HAlign_Right)
								[
									// Launch Simulated Players Deployment Button
									SNew(SUniformGridPanel)
									.SlotPadding(FMargin(2.0f, 0.0f, 0.0f, 0.0f))
									+ SUniformGridPanel::Slot(0, 0)
									[
										SNew(SButton)
										.HAlign(HAlign_Center)
										.Text(FText::FromString(FString(TEXT("Launch Deployment"))))
										.OnClicked(this, &SSpatialGDKSimulatedPlayerDeployment::OnLaunchClicked)
										.IsEnabled(this, &SSpatialGDKSimulatedPlayerDeployment::IsDeploymentConfigurationValid)
									]
									// Stop Cloud Deployment Button
									+ SUniformGridPanel::Slot(1, 0)
									[
										SNew(SButton)
										.HAlign(HAlign_Center)
										.Text(FText::FromString(FString(TEXT("Stop Deployments"))))
										.OnClicked(this, &SSpatialGDKSimulatedPlayerDeployment::OnStopClicked)
									]
								]
							]
						]
					]
				]
			]
		];
}

void SSpatialGDKSimulatedPlayerDeployment::OnDeploymentAssemblyCommited(const FText& InText, ETextCommit::Type InCommitType)
{
	USpatialGDKEditorCloudLauncherSettings* SpatialGDKCloudLauncherSettings = GetMutableDefault<USpatialGDKEditorCloudLauncherSettings>();
	SpatialGDKCloudLauncherSettings->SetAssemblyName(InText.ToString());
}

void SSpatialGDKSimulatedPlayerDeployment::OnProjectNameCommited(const FText& InText, ETextCommit::Type InCommitType)
{
	USpatialGDKEditorCloudLauncherSettings* SpatialGDKCloudLauncherSettings = GetMutableDefault<USpatialGDKEditorCloudLauncherSettings>();
	SpatialGDKCloudLauncherSettings->SetProjectName(InText.ToString());
}

void SSpatialGDKSimulatedPlayerDeployment::OnPrimaryDeploymentNameCommited(const FText& InText, ETextCommit::Type InCommitType)
{
	USpatialGDKEditorCloudLauncherSettings* SpatialGDKCloudLauncherSettings = GetMutableDefault<USpatialGDKEditorCloudLauncherSettings>();
	SpatialGDKCloudLauncherSettings->SetPrimaryDeploymentName(InText.ToString());
}

void SSpatialGDKSimulatedPlayerDeployment::OnSnapshotPathPicked(const FString& PickedPath)
{
	USpatialGDKEditorCloudLauncherSettings* SpatialGDKCloudLauncherSettings = GetMutableDefault<USpatialGDKEditorCloudLauncherSettings>();
	SpatialGDKCloudLauncherSettings->SetSnapshotPath(PickedPath);
}

void SSpatialGDKSimulatedPlayerDeployment::OnPrimaryLaunchConfigPathPicked(const FString& PickedPath)
{
	USpatialGDKEditorCloudLauncherSettings* SpatialGDKCloudLauncherSettings = GetMutableDefault<USpatialGDKEditorCloudLauncherSettings>();
	SpatialGDKCloudLauncherSettings->SetPrimaryLaunchConfigPath(PickedPath);
}

void SSpatialGDKSimulatedPlayerDeployment::OnSimulatedPlayerDeploymentNameCommited(const FText& InText, ETextCommit::Type InCommitType)
{
	USpatialGDKEditorCloudLauncherSettings* SpatialGDKCloudLauncherSettings = GetMutableDefault<USpatialGDKEditorCloudLauncherSettings>();
	SpatialGDKCloudLauncherSettings->SetSimulatedPlayerDeploymentName(InText.ToString());
}

void SSpatialGDKSimulatedPlayerDeployment::OnNumberOfSimulatedPlayersCommited(uint32 NewValue)
{
	USpatialGDKEditorCloudLauncherSettings* SpatialGDKCloudLauncherSettings = GetMutableDefault<USpatialGDKEditorCloudLauncherSettings>();
	SpatialGDKCloudLauncherSettings->SetNumberOfSimulatedPlayers(NewValue);
}

void SSpatialGDKSimulatedPlayerDeployment::OnSimulatedPlayerLaunchConfigPathPicked(const FString& PickedPath)
{
	USpatialGDKEditorCloudLauncherSettings* SpatialGDKCloudLauncherSettings = GetMutableDefault<USpatialGDKEditorCloudLauncherSettings>();
	SpatialGDKCloudLauncherSettings->SetSimulatedPlayerLaunchConfigPath(PickedPath);
}

FReply SSpatialGDKSimulatedPlayerDeployment::OnLaunchClicked()
{
	const USpatialGDKEditorCloudLauncherSettings* SpatialGDKCloudLauncherSettings = GetDefault<USpatialGDKEditorCloudLauncherSettings>();

	if (!SpatialGDKCloudLauncherSettings->IsDeploymentConfigurationValid()) {
		FNotificationInfo Info(FText::FromString(TEXT("Deployment configuration is not valid.")));
		Info.bUseSuccessFailIcons = true;
		Info.ExpireDuration = 3.0f;

		TSharedPtr<SNotificationItem> NotificationItem = FSlateNotificationManager::Get().AddNotification(Info);
		NotificationItem->SetCompletionState(SNotificationItem::CS_Fail);

		return FReply::Handled();
	}

	if (TSharedPtr<FSpatialGDKEditor> SpatialGDKEditorSharedPtr = SpatialGDKEditorPtr.Pin()) {
		FNotificationInfo Info(FText::FromString(TEXT("Starting simulated player deployment...")));
		Info.bUseSuccessFailIcons = true;
		Info.bFireAndForget = false;

		TSharedPtr<SNotificationItem> NotificationItem = FSlateNotificationManager::Get().AddNotification(Info);

		NotificationItem->SetCompletionState(SNotificationItem::CS_Pending);

		SpatialGDKEditorSharedPtr->LaunchCloudDeployment(
			FSimpleDelegate::CreateLambda([NotificationItem]() {
				NotificationItem->SetText(FText::FromString(TEXT("Successfully initiated launching of the cloud deployment.")));
				NotificationItem->SetCompletionState(SNotificationItem::CS_Success);
			}),
			FSimpleDelegate::CreateLambda([NotificationItem]() {
				NotificationItem->SetText(FText::FromString(TEXT("Failed to launch the DeploymentLauncher script properly.")));
				NotificationItem->SetCompletionState(SNotificationItem::CS_Fail);
			})
		);
		return FReply::Handled();
	}

	FNotificationInfo Info(FText::FromString(TEXT("Couldn't launch the deployment.")));
	Info.bUseSuccessFailIcons = true;
	Info.ExpireDuration = 3.0f;

	TSharedPtr<SNotificationItem> NotificationItem = FSlateNotificationManager::Get().AddNotification(Info);
	NotificationItem->SetCompletionState(SNotificationItem::CS_Fail);

	return FReply::Handled();
}

FReply SSpatialGDKSimulatedPlayerDeployment::OnRefreshClicked()
{
	// TODO: Invoke the Deployment Launcher script to list the deployments
	return FReply::Handled();
}

FReply SSpatialGDKSimulatedPlayerDeployment::OnStopClicked()
{
	if (TSharedPtr<FSpatialGDKEditor> SpatialGDKEditorSharedPtr = SpatialGDKEditorPtr.Pin()) {
		FNotificationInfo Info(FText::FromString(TEXT("Stopping cloud deployment ...")));
		Info.bUseSuccessFailIcons = true;
		Info.bFireAndForget = false;

		TSharedPtr<SNotificationItem> NotificationItem = FSlateNotificationManager::Get().AddNotification(Info);

		NotificationItem->SetCompletionState(SNotificationItem::CS_Pending);

		SpatialGDKEditorSharedPtr->StopCloudDeployment(
				FSimpleDelegate::CreateLambda([NotificationItem]() {
				NotificationItem->SetText(FText::FromString(TEXT("Successfully launched the stop cloud deployments command.")));
				NotificationItem->SetCompletionState(SNotificationItem::CS_Success);
			}),
				FSimpleDelegate::CreateLambda([NotificationItem]() {
				NotificationItem->SetText(FText::FromString(TEXT("Failed to launch the DeploymentLauncher script properly.")));
				NotificationItem->SetCompletionState(SNotificationItem::CS_Fail);
			})
		);
	}
	return FReply::Handled();
}

void SSpatialGDKSimulatedPlayerDeployment::OnCloudDocumentationClicked()
{
	FString WebError;
	FPlatformProcess::LaunchURL(TEXT("https://docs.improbable.io/unreal/alpha/content/get-started/tutorial#build-your-assemblies"), TEXT(""), &WebError);
	if (!WebError.IsEmpty())
	{
		FNotificationInfo Info(FText::FromString(WebError));
		Info.ExpireDuration = 3.0f;
		Info.bUseSuccessFailIcons = true;
		TSharedPtr<SNotificationItem> NotificationItem = FSlateNotificationManager::Get().AddNotification(Info);
		NotificationItem->SetCompletionState(SNotificationItem::CS_Fail);
		NotificationItem->ExpireAndFadeout();
	}
}

void SSpatialGDKSimulatedPlayerDeployment::OnCheckedSimulatedPlayers(ECheckBoxState NewCheckedState)
{
	USpatialGDKEditorCloudLauncherSettings* SpatialGDKCloudLauncherSettings = GetMutableDefault<USpatialGDKEditorCloudLauncherSettings>();
	SpatialGDKCloudLauncherSettings->SetSimulatedPlayersEnabledState(NewCheckedState == ECheckBoxState::Checked);
}

ECheckBoxState SSpatialGDKSimulatedPlayerDeployment::IsSimulatedPlayersEnabled() const
{
	const USpatialGDKEditorCloudLauncherSettings* SpatialGDKCloudLauncherSettings = GetDefault<USpatialGDKEditorCloudLauncherSettings>();
	return SpatialGDKCloudLauncherSettings->IsSimulatedPlayersEnabled() ? ECheckBoxState::Checked : ECheckBoxState::Unchecked;
}

bool SSpatialGDKSimulatedPlayerDeployment::IsDeploymentConfigurationValid() const
{
	return true;
}
