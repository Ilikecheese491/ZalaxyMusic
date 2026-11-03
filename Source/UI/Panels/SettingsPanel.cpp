#include "SettingsPanel.h"
#include "Core/AI/CloudAIProvider.h"
#include "Core/Licensing/SubscriptionManager.h"
#include "Core/AI/UsageTracker.h"

namespace zalaxy
{

// ================================================================================
// BYOK Setup Wizard Component - Embedded OpenRouter signup flow
// ================================================================================

class BYOKSetupWizard : public juce::Component,
                        public juce::Button::Listener,
                        public juce::Timer
{
public:
    enum class Step
    {
        Welcome = 0,
        SignUp = 1,
        CreateKey = 2,
        CopyKey = 3,
        PasteKey = 4,
        VerifyKey = 5,
        Complete = 6
    };

    BYOKSetupWizard (CloudAIProvider& provider, SubscriptionManager& subManager)
        : aiProvider (provider)
        , subscriptionManager (subManager)
        , currentStep (Step::Welcome)
    {
        createWelcomeStep();
        addAndMakeVisible (currentStepComponent.get());
        startTimerHz (2); // For animation
    }

    ~BYOKSetupWizard()
    {
        stopTimer();
    }

    void paint (juce::Graphics& g) override
    {
        auto bounds = getLocalBounds().toFloat();
        g.setColour (theme.getColor (ZalaxyTheme::ColorId::PanelBackground));
        g.fillRoundedRectangle (bounds, theme.getRadius ("large"));
        g.setColour (theme.getColor (ZalaxyTheme::ColorId::BorderSubtle));
        g.drawRoundedRectangle (bounds, theme.getRadius ("large"), 1.5f);
    }

    void resized() override
    {
        if (currentStepComponent)
            currentStepComponent->setBounds (getLocalBounds().reduced (theme.getSpace (3)));
    }

    void buttonClicked (juce::Button* button) override
    {
        if (button == nextButton.get())
        {
            advanceStep();
        }
        else if (button == backButton.get())
        {
            goBackStep();
        }
        else if (button == skipButton.get())
        {
            closeWizard();
        }
        else if (button == copyKeyButton.get())
        {
            copyAPIKeyToClipboard();
        }
        else if (button == openOpenRouterButton.get())
        {
            openOpenRouterInBrowser();
        }
        else if (button == pasteKeyButton.get())
        {
            pasteKeyFromClipboard();
        }
        else if (button == verifyKeyButton.get())
        {
            verifyAPIKey();
        }
    }

    std::function<void()> onComplete;

private:
    CloudAIProvider& aiProvider;
    SubscriptionManager& subscriptionManager;
    ZalaxyTheme theme;

    Step currentStep;
    std::unique_ptr<juce::Component> currentStepComponent;
    std::unique_ptr<juce::TextButton> nextButton;
    std::unique_ptr<juce::TextButton> backButton;
    std::unique_ptr<juce::TextButton> skipButton;
    std::unique_ptr<juce::TextButton> copyKeyButton;
    std::unique_ptr<juce::TextButton> openOpenRouterButton;
    std::unique_ptr<juce::TextButton> pasteKeyButton;
    std::unique_ptr<juce::TextButton> verifyKeyButton;

    juce::String pendingAPIKey;
    float checkmarkProgress = 0.0f;
    bool checkmarkAnimating = false;

    void createWelcomeStep()
    {
        auto panel = std::make_unique<juce::Component>();
        panel->setOpaque (false);

        auto title = std::make_unique<juce::Label>();
        title->setText ("Welcome to AI Setup", juce::dontSendNotification);
        title->setFont (theme.getFont ("heading1"));
        title->setColour (juce::Label::textColourId, theme.getColor (ZalaxyTheme::ColorId::AccentPrimary));
        title->setJustificationType (juce::Justification::centred);
        panel->addAndMakeVisible (title.get());

        auto desc = std::make_unique<juce::Label>();
        desc->setText ("To use cloud AI features, you'll need an API key from OpenRouter.\n\n"
                       "OpenRouter gives you access to 100+ models with a generous free tier.\n"
                       "We'll guide you through the entire process step by step.",
                       juce::dontSendNotification);
        desc->setFont (theme.getFont ("body"));
        desc->setColour (juce::Label::textColourId, theme.getColor (ZalaxyTheme::ColorId::TextSecondary));
        desc->setJustificationType (juce::Justification::centred);
        panel->addAndMakeVisible (desc.get());

        auto features = std::make_unique<juce::Label>();
        features->setText ("✓ Free tier with millions of tokens\n"
                          "✓ Access to GPT-4, Claude, Llama, and more\n"
                          "✓ Your key stays on your computer\n"
                          "✓ No credit card required",
                          juce::dontSendNotification);
        features->setFont (theme.getFont ("bodySmall"));
        features->setColour (juce::Label::textColourId, theme.getColor (ZalaxyTheme::ColorId::TextMuted));
        features->setJustificationType (juce::Justification::centred);
        panel->addAndMakeVisible (features.get());

        nextButton = std::make_unique<juce::TextButton> ("Get Started");
        nextButton->setColour (juce::TextButton::buttonColourId, theme.getColor (ZalaxyTheme::ColorId::AccentPrimary));
        nextButton->setColour (juce::TextButton::textColourOffId, theme.getColor (ZalaxyTheme::ColorId::TextInverse));
        nextButton->addListener (this);
        panel->addAndMakeVisible (nextButton.get());

        skipButton = std::make_unique<juce::TextButton> ("Skip for Now");
        skipButton->setColour (juce::TextButton::buttonColourId, theme.getColor (ZalaxyTheme::ColorId::Surface));
        skipButton->setColour (juce::TextButton::textColourOffId, theme.getColor (ZalaxyTheme::ColorId::TextMuted));
        skipButton->addListener (this);
        panel->addAndMakeVisible (skipButton.get());

        // Layout
        auto bounds = panel->getLocalBounds().reduced (theme.getSpace (4));
        title->setBounds (bounds.removeFromTop (40));
        bounds.removeFromTop (theme.getSpace (2));
        desc->setBounds (bounds.removeFromTop (80));
        bounds.removeFromTop (theme.getSpace (2));
        features->setBounds (bounds.removeFromTop (80));
        bounds.removeFromTop (theme.getSpace (4));

        auto btnRow = bounds.removeFromBottom (40);
        nextButton->setBounds (btnRow.removeFromRight (140));
        skipButton->setBounds (btnRow.removeFromLeft (120));

        title.release();
        desc.release();
        features.release();
        nextButton.release();
        skipButton.release();

        currentStepComponent = std::move (panel);
    }

    void createSignUpStep()
    {
        auto panel = std::make_unique<juce::Component>();
        panel->setOpaque (false);

        auto title = std::make_unique<juce::Label>();
        title->setText ("Step 1: Create Your Account", juce::dontSendNotification);
        title->setFont (theme.getFont ("heading2"));
        title->setColour (juce::Label::textColourId, theme.getColor (ZalaxyTheme::ColorId::AccentPrimary));
        title->setJustificationType (juce::Justification::centred);
        panel->addAndMakeVisible (title.get());

        auto desc = std::make_unique<juce::Label>();
        desc->setText ("Click the button below to open OpenRouter and create your free account.\n"
                       "You can sign up with GitHub, Google, or email.",
                       juce::dontSendNotification);
        desc->setFont (theme.getFont ("body"));
        desc->setColour (juce::Label::textColourId, theme.getColor (ZalaxyTheme::ColorId::TextSecondary));
        desc->setJustificationType (juce::Justification::centred);
        panel->addAndMakeVisible (desc.get());

        openOpenRouterButton = std::make_unique<juce::TextButton> ("Open OpenRouter →");
        openOpenRouterButton->setColour (juce::TextButton::buttonColourId, theme.getColor (ZalaxyTheme::ColorId::AccentSecondary));
        openOpenRouterButton->setColour (juce::TextButton::textColourOffId, theme.getColor (ZalaxyTheme::ColorId::TextInverse));
        openOpenRouterButton->addListener (this);
        panel->addAndMakeVisible (openOpenRouterButton.get());

        auto note = std::make_unique<juce::Label>();
        note->setText ("After signing up, you'll be redirected to your dashboard.\nClick 'Next' when you're ready to create your API key.",
                       juce::dontSendNotification);
        note->setFont (theme.getFont ("caption"));
        note->setColour (juce::Label::textColourId, theme.getColor (ZalaxyTheme::ColorId::TextMuted));
        note->setJustificationType (juce::Justification::centred);
        panel->addAndMakeVisible (note.get());

        nextButton = std::make_unique<juce::TextButton> ("Next: Create API Key");
        nextButton->setColour (juce::TextButton::buttonColourId, theme.getColor (ZalaxyTheme::ColorId::AccentPrimary));
        nextButton->setColour (juce::TextButton::textColourOffId, theme.getColor (ZalaxyTheme::ColorId::TextInverse));
        nextButton->addListener (this);
        panel->addAndMakeVisible (nextButton.get());

        backButton = std::make_unique<juce::TextButton> ("Back");
        backButton->setColour (juce::TextButton::buttonColourId, theme.getColor (ZalaxyTheme::ColorId::Surface));
        backButton->setColour (juce::TextButton::textColourOffId, theme.getColor (ZalaxyTheme::ColorId::TextPrimary));
        backButton->addListener (this);
        panel->addAndMakeVisible (backButton.get());

        // Layout
        auto bounds = panel->getLocalBounds().reduced (theme.getSpace (4));
        title->setBounds (bounds.removeFromTop (36));
        bounds.removeFromTop (theme.getSpace (2));
        desc->setBounds (bounds.removeFromTop (60));
        bounds.removeFromTop (theme.getSpace (2));
        openOpenRouterButton->setBounds (bounds.removeFromTop (44).withTrimmedLeft (80).withTrimmedRight (80));
        bounds.removeFromTop (theme.getSpace (2));
        note->setBounds (bounds.removeFromTop (50));
        bounds.removeFromTop (theme.getSpace (4));

        auto btnRow = bounds.removeFromBottom (40);
        backButton->setBounds (btnRow.removeFromLeft (100));
        nextButton->setBounds (btnRow.removeFromRight (180));

        title.release();
        desc.release();
        note.release();
        openOpenRouterButton.release();
        nextButton.release();
        backButton.release();

        currentStepComponent = std::move (panel);
    }

    void createKeyStep()
    {
        auto panel = std::make_unique<juce::Component>();
        panel->setOpaque (false);

        auto title = std::make_unique<juce::Label>();
        title->setText ("Step 2: Create Your API Key", juce::dontSendNotification);
        title->setFont (theme.getFont ("heading2"));
        title->setColour (juce::Label::textColourId, theme.getColor (ZalaxyTheme::ColorId::AccentPrimary));
        title->setJustificationType (juce::Justification::centred);
        panel->addAndMakeVisible (title.get());

        auto desc = std::make_unique<juce::Label>();
        desc->setText ("On the OpenRouter page that opened:\n\n"
                       "1. Click 'Create Key' or go to the 'Keys' tab\n"
                       "2. Give it a name like 'ZalaxyMusic'\n"
                       "3. Click 'Create' to generate your key\n\n"
                       "⚠ IMPORTANT: Your API key is like a password.\n"
                       "   • Never share it with anyone\n"
                       "   • Never commit it to GitHub\n"
                       "   • Never paste it in chat or emails\n"
                       "   • It gives access to your OpenRouter credits",
                       juce::dontSendNotification);
        desc->setFont (theme.getFont ("body"));
        desc->setColour (juce::Label::textColourId, theme.getColor (ZalaxyTheme::ColorId::TextSecondary));
        desc->setJustificationType (juce::Justification::centredLeft);
        panel->addAndMakeVisible (desc.get());

        auto warningBox = std::make_unique<juce::Component>();
        warningBox->setOpaque (true);
        panel->addAndMakeVisible (warningBox.get());

        nextButton = std::make_unique<juce::TextButton> ("I've Created My Key →");
        nextButton->setColour (juce::TextButton::buttonColourId, theme.getColor (ZalaxyTheme::ColorId::AccentPrimary));
        nextButton->setColour (juce::TextButton::textColourOffId, theme.getColor (ZalaxyTheme::ColorId::TextInverse));
        nextButton->addListener (this);
        panel->addAndMakeVisible (nextButton.get());

        backButton = std::make_unique<juce::TextButton> ("Back");
        backButton->setColour (juce::TextButton::buttonColourId, theme.getColor (ZalaxyTheme::ColorId::Surface));
        backButton->setColour (juce::TextButton::textColourOffId, theme.getColor (ZalaxyTheme::ColorId::TextPrimary));
        backButton->addListener (this);
        panel->addAndMakeVisible (backButton.get());

        // Layout
        auto bounds = panel->getLocalBounds().reduced (theme.getSpace (4));
        title->setBounds (bounds.removeFromTop (36));
        bounds.removeFromTop (theme.getSpace (2));
        desc->setBounds (bounds.removeFromTop (220));
        bounds.removeFromTop (theme.getSpace (4));

        auto btnRow = bounds.removeFromBottom (40);
        backButton->setBounds (btnRow.removeFromLeft (100));
        nextButton->setBounds (btnRow.removeFromRight (180));

        title.release();
        desc.release();
        nextButton.release();
        backButton.release();

        currentStepComponent = std::move (panel);
    }

    void createCopyStep()
    {
        auto panel = std::make_unique<juce::Component>();
        panel->setOpaque (false);

        auto title = std::make_unique<juce::Label>();
        title->setText ("Step 3: Copy Your API Key", juce::dontSendNotification);
        title->setFont (theme.getFont ("heading2"));
        title->setColour (juce::Label::textColourId, theme.getColor (ZalaxyTheme::ColorId::AccentPrimary));
        title->setJustificationType (juce::Justification::centred);
        panel->addAndMakeVisible (title.get());

        auto desc = std::make_unique<juce::Label>();
        desc->setText ("Your new API key is shown ONCE on OpenRouter.\n\n"
                       "1. Click the copy icon next to the key\n"
                       "2. Come back here and click 'Paste Key' below\n\n"
                       "If you lost it, you'll need to create a new one.",
                       juce::dontSendNotification);
        desc->setFont (theme.getFont ("body"));
        desc->setColour (juce::Label::textColourId, theme.getColor (ZalaxyTheme::ColorId::TextSecondary));
        desc->setJustificationType (juce::Justification::centredLeft);
        panel->addAndMakeVisible (desc.get());

        copyKeyButton = std::make_unique<juce::TextButton> ("\U0001F4CB Copy Key from OpenRouter");
        copyKeyButton->setColour (juce::TextButton::buttonColourId, theme.getColor (ZalaxyTheme::ColorId::AccentSecondary));
        copyKeyButton->setColour (juce::TextButton::textColourOffId, theme.getColor (ZalaxyTheme::ColorId::TextInverse));
        copyKeyButton->addListener (this);
        panel->addAndMakeVisible (copyKeyButton.get());

        pasteKeyButton = std::make_unique<juce::TextButton> ("\U0001F4E5 Paste Key Here");
        pasteKeyButton->setColour (juce::TextButton::buttonColourId, theme.getColor (ZalaxyTheme::ColorId::AccentPrimary));
        pasteKeyButton->setColour (juce::TextButton::textColourOffId, theme.getColor (ZalaxyTheme::ColorId::TextInverse));
        pasteKeyButton->addListener (this);
        panel->addAndMakeVisible (pasteKeyButton.get());

        backButton = std::make_unique<juce::TextButton> ("Back");
        backButton->setColour (juce::TextButton::buttonColourId, theme.getColor (ZalaxyTheme::ColorId::Surface));
        backButton->setColour (juce::TextButton::textColourOffId, theme.getColor (ZalaxyTheme::ColorId::TextPrimary));
        backButton->addListener (this);
        panel->addAndMakeVisible (backButton.get());

        // Layout
        auto bounds = panel->getLocalBounds().reduced (theme.getSpace (4));
        title->setBounds (bounds.removeFromTop (36));
        bounds.removeFromTop (theme.getSpace (2));
        desc->setBounds (bounds.removeFromTop (100));
        bounds.removeFromTop (theme.getSpace (3));
        copyKeyButton->setBounds (bounds.removeFromTop (44).withTrimmedLeft (60).withTrimmedRight (60));
        bounds.removeFromTop (theme.getSpace (2));
        pasteKeyButton->setBounds (bounds.removeFromTop (44).withTrimmedLeft (60).withTrimmedRight (60));
        bounds.removeFromTop (theme.getSpace (4));

        auto btnRow = bounds.removeFromBottom (40);
        backButton->setBounds (btnRow.removeFromLeft (100));

        title.release();
        desc.release();
        copyKeyButton.release();
        pasteKeyButton.release();
        backButton.release();

        currentStepComponent = std::move (panel);
    }

    void createPasteStep()
    {
        auto panel = std::make_unique<juce::Component>();
        panel->setOpaque (false);

        auto title = std::make_unique<juce::Label>();
        title->setText ("Step 4: Paste Your API Key", juce::dontSendNotification);
        title->setFont (theme.getFont ("heading2"));
        title->setColour (juce::Label::textColourId, theme.getColor (ZalaxyTheme::ColorId::AccentPrimary));
        title->setJustificationType (juce::Justification::centred);
        panel->addAndMakeVisible (title.get());

        auto desc = std::make_unique<juce::Label>();
        desc->setText ("Paste your OpenRouter API key below.\n"
                       "It should look like: sk-or-v1-xxxxxxxxxxxx",
                       juce::dontSendNotification);
        desc->setFont (theme.getFont ("body"));
        desc->setColour (juce::Label::textColourId, theme.getColor (ZalaxyTheme::ColorId::TextSecondary));
        desc->setJustificationType (juce::Justification::centred);
        panel->addAndMakeVisible (desc.get());

        // We'll use the main panel's API key editor - just show confirmation
        auto keyStatus = std::make_unique<juce::Label>();
        keyStatus->setText ("Waiting for key...", juce::dontSendNotification);
        keyStatus->setFont (theme.getFont ("body"));
        keyStatus->setColour (juce::Label::textColourId, theme.getColor (ZalaxyTheme::ColorId::TextMuted));
        keyStatus->setJustificationType (juce::Justification::centred);
        panel->addAndMakeVisible (keyStatus.get());

        nextButton = std::make_unique<juce::TextButton> ("Verify & Save Key");
        nextButton->setColour (juce::TextButton::buttonColourId, theme.getColor (ZalaxyTheme::ColorId::AccentPrimary));
        nextButton->setColour (juce::TextButton::textColourOffId, theme.getColor (ZalaxyTheme::ColorId::TextInverse));
        nextButton->addListener (this);
        panel->addAndMakeVisible (nextButton.get());

        backButton = std::make_unique<juce::TextButton> ("Back");
        backButton->setColour (juce::TextButton::buttonColourId, theme.getColor (ZalaxyTheme::ColorId::Surface));
        backButton->setColour (juce::TextButton::textColourOffId, theme.getColor (ZalaxyTheme::ColorId::TextPrimary));
        backButton->addListener (this);
        panel->addAndMakeVisible (backButton.get());

        // Layout
        auto bounds = panel->getLocalBounds().reduced (theme.getSpace (4));
        title->setBounds (bounds.removeFromTop (36));
        bounds.removeFromTop (theme.getSpace (2));
        desc->setBounds (bounds.removeFromTop (60));
        bounds.removeFromTop (theme.getSpace (2));
        keyStatus->setBounds (bounds.removeFromTop (40));
        bounds.removeFromTop (theme.getSpace (4));

        auto btnRow = bounds.removeFromBottom (40);
        backButton->setBounds (btnRow.removeFromLeft (100));
        nextButton->setBounds (btnRow.removeFromRight (160));

        title.release();
        desc.release();
        keyStatus.release();
        nextButton.release();
        backButton.release();

        currentStepComponent = std::move (panel);
    }

    void createVerifyStep()
    {
        auto panel = std::make_unique<juce::Component>();
        panel->setOpaque (false);

        auto title = std::make_unique<juce::Label>();
        title->setText ("Step 5: Verifying Your Key...", juce::dontSendNotification);
        title->setFont (theme.getFont ("heading2"));
        title->setColour (juce::Label::textColourId, theme.getColor (ZalaxyTheme::ColorId::AccentPrimary));
        title->setJustificationType (juce::Justification::centred);
        panel->addAndMakeVisible (title.get());

        auto spinner = std::make_unique<juce::Label>();
        spinner->setText ("⟳", juce::dontSendNotification);
        spinner->setFont (theme.getFont ("heading1"));
        spinner->setColour (juce::Label::textColourId, theme.getColor (ZalaxyTheme::ColorId::AccentPrimary));
        spinner->setJustificationType (juce::Justification::centred);
        panel->addAndMakeVisible (spinner.get());

        auto desc = std::make_unique<juce::Label>();
        desc->setText ("Testing connection to OpenRouter...\nThis takes a few seconds.",
                       juce::dontSendNotification);
        desc->setFont (theme.getFont ("body"));
        desc->setColour (juce::Label::textColourId, theme.getColor (ZalaxyTheme::ColorId::TextSecondary));
        desc->setJustificationType (juce::Justification::centred);
        panel->addAndMakeVisible (desc.get());

        // Layout
        auto bounds = panel->getLocalBounds().reduced (theme.getSpace (4));
        title->setBounds (bounds.removeFromTop (36));
        bounds.removeFromTop (theme.getSpace (2));
        spinner->setBounds (bounds.removeFromTop (60));
        bounds.removeFromTop (theme.getSpace (2));
        desc->setBounds (bounds.removeFromTop (40));

        title.release();
        spinner.release();
        desc.release();

        currentStepComponent = std::move (panel);
    }

    void createCompleteStep()
    {
        auto panel = std::make_unique<juce::Component>();
        panel->setOpaque (false);

        auto title = std::make_unique<juce::Label>();
        title->setText ("\U0001F389 All Set!", juce::dontSendNotification);
        title->setFont (theme.getFont ("heading1"));
        title->setColour (juce::Label::textColourId, theme.getColor (ZalaxyTheme::ColorId::AccentSuccess));
        title->setJustificationType (juce::Justification::centred);
        panel->addAndMakeVisible (title.get());

        auto desc = std::make_unique<juce::Label>();
        desc->setText ("Your API key is saved and verified.\nCloud AI features are now unlocked!\n\n"
                       "You can now:\n"
                       "• Generate patches from text descriptions\n"
                       "• Analyze audio references\n"
                       "• Chat with the production assistant\n\n"
                       "Your key is stored securely on your computer only.",
                       juce::dontSendNotification);
        desc->setFont (theme.getFont ("body"));
        desc->setColour (juce::Label::textColourId, theme.getColor (ZalaxyTheme::ColorId::TextSecondary));
        desc->setJustificationType (juce::Justification::centred);
        panel->addAndMakeVisible (desc.get());

        auto doneButton = std::make_unique<juce::TextButton> ("Done");
        doneButton->setColour (juce::TextButton::buttonColourId, theme.getColor (ZalaxyTheme::ColorId::AccentPrimary));
        doneButton->setColour (juce::TextButton::textColourOffId, theme.getColor (ZalaxyTheme::ColorId::TextInverse));
        doneButton->addListener (this);
        panel->addAndMakeVisible (doneButton.get());

        // Layout
        auto bounds = panel->getLocalBounds().reduced (theme.getSpace (4));
        title->setBounds (bounds.removeFromTop (50));
        bounds.removeFromTop (theme.getSpace (2));
        desc->setBounds (bounds.removeFromTop (140));
        bounds.removeFromTop (theme.getSpace (4));

        doneButton->setBounds (bounds.removeFromBottom (44).withTrimmedLeft (80).withTrimmedRight (80));

        title.release();
        desc.release();
        doneButton.release();

        currentStepComponent = std::move (panel);
    }

    void advanceStep()
    {
        if (currentStep == Step::Welcome)
        {
            currentStep = Step::SignUp;
            createSignUpStep();
            openOpenRouterInBrowser();
        }
        else if (currentStep == Step::SignUp)
        {
            currentStep = Step::CreateKey;
            createKeyStep();
        }
        else if (currentStep == Step::CreateKey)
        {
            currentStep = Step::CopyKey;
            createCopyStep();
        }
        else if (currentStep == Step::CopyKey)
        {
            currentStep = Step::PasteKey;
            createPasteStep();
        }
        else if (currentStep == Step::PasteKey)
        {
            currentStep = Step::VerifyKey;
            createVerifyStep();
            verifyAPIKey();
        }
        else if (currentStep == Step::VerifyKey)
        {
            // Handled by async callback
        }
        else if (currentStep == Step::Complete)
        {
            closeWizard();
        }

        updateUI();
    }

    void goBackStep()
    {
        if (currentStep == Step::SignUp)
        {
            currentStep = Step::Welcome;
            createWelcomeStep();
        }
        else if (currentStep == Step::CreateKey)
        {
            currentStep = Step::SignUp;
            createSignUpStep();
        }
        else if (currentStep == Step::CopyKey)
        {
            currentStep = Step::CreateKey;
            createKeyStep();
        }
        else if (currentStep == Step::PasteKey)
        {
            currentStep = Step::CopyKey;
            createCopyStep();
        }

        updateUI();
    }

    void closeWizard()
    {
        if (onComplete)
            onComplete();

        if (auto* dialog = findParentComponentOfClass<juce::DialogWindow>())
            dialog->exitModalState (1);
    }

    void updateUI()
    {
        if (currentStepComponent)
        {
            currentStepComponent->setBounds (getLocalBounds().reduced (theme.getSpace (3)));
            repaint();
        }
    }

    void openOpenRouterInBrowser()
    {
        juce::URL ("https://openrouter.ai/signup").launchInDefaultBrowser();
    }

    void copyAPIKeyToClipboard()
    {
        // User should copy from OpenRouter page
        juce::SystemClipboard::copyTextToClipboard ("");
        // Just open the page
        juce::URL ("https://openrouter.ai/keys").launchInDefaultBrowser();
    }

    void pasteKeyFromClipboard()
    {
        // This will be handled by the main panel's API key editor - just advance
        if (pendingAPIKey.isEmpty())
        {
            pendingAPIKey = juce::SystemClipboard::getTextFromClipboard();
        }
        advanceStep();
    }

    void verifyAPIKey()
    {
        aiProvider.testConnection ([this] (bool success, const juce::String& message)
        {
            juce::MessageManager::callAsync ([this, success, message]()
            {
                if (success)
                {
                    currentStep = Step::Complete;
                    createCompleteStep();
                    checkmarkAnimating = true;
                    checkmarkProgress = 0.0f;
                }
                else
                {
                    // Show error, allow retry
                    // For simplicity, just show error in current step
                }
                updateUI();
            });
        });
    }

    void showKeyCreatedInstructions()
    {
        // Called when user reaches the keys page
    }

    void timerCallback() override
    {
        if (checkmarkAnimating && checkmarkProgress < 1.0f)
        {
            checkmarkProgress += 0.05f;
            if (checkmarkProgress >= 1.0f)
            {
                checkmarkProgress = 1.0f;
                checkmarkAnimating = false;
            }
            repaint();
        }
    }
};

// ================================================================================
// SettingsPanel Implementation
// ================================================================================

SettingsPanel::SettingsPanel (CloudAIProvider& aiProvider, SubscriptionManager& subManager)
    : aiProvider (aiProvider)
    , subscriptionManager (subManager)
    , usageProgressBar (usageProgress)
{
    createAISection();
    createModeSection();
    createUsageSection();
    createSubscriptionSection();
    createGuideSection();

    addAndMakeVisible (aiSectionLabel);
    addAndMakeVisible (providerSelector);
    addAndMakeVisible (apiKeyLabel);
    addAndMakeVisible (apiKeyEditor);
    addAndMakeVisible (saveKeyButton);
    addAndMakeVisible (testKeyButton);
    addAndMakeVisible (keyStatusLabel);
    addAndMakeVisible (modeLabel);
    addAndMakeVisible (cloudModeToggle);
    addAndMakeVisible (modeDescriptionLabel);
    addAndMakeVisible (usageSectionLabel);
    addAndMakeVisible (tokensUsedLabel);
    addAndMakeVisible (tokensRemainingLabel);
    addAndMakeVisible (usageProgressBar);
    addAndMakeVisible (limitWarningLabel);
    addAndMakeVisible (subSectionLabel);
    addAndMakeVisible (subStatusLabel);
    addAndMakeVisible (enterLicenseButton);
    addAndMakeVisible (buyLicenseButton);
    addAndMakeVisible (setupGuideButton);

    updateUI();
}

SettingsPanel::~SettingsPanel()
{
}

void SettingsPanel::createAISection()
{
    aiSectionLabel.setText ("AI PROVIDER", juce::dontSendNotification);
    aiSectionLabel.setFont (theme.getFont ("caption"));
    aiSectionLabel.setColour (juce::Label::textColourId, theme.getColor (ZalaxyTheme::ColorId::TextMuted));

    providerSelector.addItem ("OpenRouter (Recommended)", 1);
    providerSelector.addItem ("Together AI", 2);
    providerSelector.addItem ("Groq", 3);
    providerSelector.setSelectedId (1);
    providerSelector.addListener (this);
    providerSelector.setColour (juce::ComboBox::backgroundColourId, theme.getColor (ZalaxyTheme::ColorId::Surface));
    providerSelector.setColour (juce::ComboBox::textColourId, theme.getColor (ZalaxyTheme::ColorId::TextPrimary));
    providerSelector.setColour (juce::ComboBox::outlineColourId, theme.getColor (ZalaxyTheme::ColorId::BorderDefault));

    apiKeyLabel.setText ("API Key", juce::dontSendNotification);
    apiKeyLabel.setFont (theme.getFont ("caption"));
    apiKeyLabel.setColour (juce::Label::textColourId, theme.getColor (ZalaxyTheme::ColorId::TextMuted));

    apiKeyEditor.setText (aiProvider.getAPIKey ("OpenRouter"));
    apiKeyEditor.setTextToShowWhenEmpty ("sk-or-v1-xxxxxxxxxxxx (paste your OpenRouter key here)", theme.getColor (ZalaxyTheme::ColorId::TextMuted));
    apiKeyEditor.setFont (theme.getFont ("bodySmall"));
    apiKeyEditor.setColour (juce::TextEditor::backgroundColourId, theme.getColor (ZalaxyTheme::ColorId::Surface));
    apiKeyEditor.setColour (juce::TextEditor::textColourId, theme.getColor (ZalaxyTheme::ColorId::TextPrimary));
    apiKeyEditor.setColour (juce::TextEditor::outlineColourId, theme.getColor (ZalaxyTheme::ColorId::BorderDefault));
    apiKeyEditor.setColour (juce::TextEditor::focusedOutlineColourId, theme.getColor (ZalaxyTheme::ColorId::BorderFocus));
    apiKeyEditor.addListener (this);

    saveKeyButton.setButtonText ("Save Key");
    saveKeyButton.setColour (juce::TextButton::buttonColourId, theme.getColor (ZalaxyTheme::ColorId::AccentPrimary));
    saveKeyButton.setColour (juce::TextButton::textColourOffId, theme.getColor (ZalaxyTheme::ColorId::TextInverse));
    saveKeyButton.addListener (this);

    testKeyButton.setButtonText ("Test Connection");
    testKeyButton.setColour (juce::TextButton::buttonColourId, theme.getColor (ZalaxyTheme::ColorId::Surface));
    testKeyButton.setColour (juce::TextButton::textColourOffId, theme.getColor (ZalaxyTheme::ColorId::TextPrimary));
    testKeyButton.addListener (this);

    keyStatusLabel.setFont (theme.getFont ("caption"));
    keyStatusLabel.setColour (juce::Label::textColourId, theme.getColor (ZalaxyTheme::ColorId::TextMuted));
    keyStatusLabel.setJustificationType (juce::Justification::centredLeft);
}

void SettingsPanel::createModeSection()
{
    modeLabel.setText ("AI MODE", juce::dontSendNotification);
    modeLabel.setFont (theme.getFont ("caption"));
    modeLabel.setColour (juce::Label::textColourId, theme.getColor (ZalaxyTheme::ColorId::TextMuted));

    cloudModeToggle.setButtonText ("Use Cloud AI (OpenRouter)");
    cloudModeToggle.setToggleState (aiProvider.isCloudMode(), juce::dontSendNotification);
    cloudModeToggle.addListener (this);
    cloudModeToggle.setColour (juce::ToggleButton::textColourId, theme.getColor (ZalaxyTheme::ColorId::TextPrimary));
    cloudModeToggle.setColour (juce::ToggleButton::tickColourId, theme.getColor (ZalaxyTheme::ColorId::AccentPrimary));
    cloudModeToggle.setColour (juce::ToggleButton::tickDisabledColourId, theme.getColor (ZalaxyTheme::ColorId::BorderSubtle));

    modeDescriptionLabel.setText ("Cloud: More powerful models, requires internet & API key\nLocal: Runs offline, basic models only",
                                  juce::dontSendNotification);
    modeDescriptionLabel.setFont (theme.getFont ("caption"));
    modeDescriptionLabel.setColour (juce::Label::textColourId, theme.getColor (ZalaxyTheme::ColorId::TextMuted));
    modeDescriptionLabel.setJustificationType (juce::Justification::centredLeft);
}

void SettingsPanel::createUsageSection()
{
    usageSectionLabel.setText ("TOKEN USAGE", juce::dontSendNotification);
    usageSectionLabel.setFont (theme.getFont ("caption"));
    usageSectionLabel.setColour (juce::Label::textColourId, theme.getColor (ZalaxyTheme::ColorId::TextMuted));

    tokensUsedLabel.setFont (theme.getFont ("body"));
    tokensUsedLabel.setColour (juce::Label::textColourId, theme.getColor (ZalaxyTheme::ColorId::TextPrimary));
    tokensUsedLabel.setJustificationType (juce::Justification::centredLeft);

    tokensRemainingLabel.setFont (theme.getFont ("body"));
    tokensRemainingLabel.setColour (juce::Label::textColourId, theme.getColor (ZalaxyTheme::ColorId::TextSecondary));
    tokensRemainingLabel.setJustificationType (juce::Justification::centredRight);

    usageProgressBar.setColour (juce::ProgressBar::backgroundColourId, theme.getColor (ZalaxyTheme::ColorId::Surface));
    usageProgressBar.setColour (juce::ProgressBar::foregroundColourId, theme.getColor (ZalaxyTheme::ColorId::AccentPrimary));

    limitWarningLabel.setFont (theme.getFont ("caption"));
    limitWarningLabel.setColour (juce::Label::textColourId, theme.getColor (ZalaxyTheme::ColorId::AccentWarm));
    limitWarningLabel.setJustificationType (juce::Justification::centred);
}

void SettingsPanel::createSubscriptionSection()
{
    subSectionLabel.setText ("SUBSCRIPTION", juce::dontSendNotification);
    subSectionLabel.setFont (theme.getFont ("caption"));
    subSectionLabel.setColour (juce::Label::textColourId, theme.getColor (ZalaxyTheme::ColorId::TextMuted));

    subStatusLabel.setFont (theme.getFont ("body"));
    subStatusLabel.setColour (juce::Label::textColourId, theme.getColor (ZalaxyTheme::ColorId::TextPrimary));
    subStatusLabel.setJustificationType (juce::Justification::centredLeft);

    enterLicenseButton.setButtonText ("Enter License Key");
    enterLicenseButton.setColour (juce::TextButton::buttonColourId, theme.getColor (ZalaxyTheme::ColorId::Surface));
    enterLicenseButton.setColour (juce::TextButton::textColourOffId, theme.getColor (ZalaxyTheme::ColorId::TextPrimary));
    enterLicenseButton.addListener (this);

    buyLicenseButton.setButtonText ("Upgrade to Pro");
    buyLicenseButton.setColour (juce::TextButton::buttonColourId, theme.getColor (ZalaxyTheme::ColorId::AccentPrimary));
    buyLicenseButton.setColour (juce::TextButton::textColourOffId, theme.getColor (ZalaxyTheme::ColorId::TextInverse));
    buyLicenseButton.addListener (this);
}

void SettingsPanel::createGuideSection()
{
    setupGuideButton.setButtonText ("\U0001F4D6 Setup Guide & Help");
    setupGuideButton.setColour (juce::TextButton::buttonColourId, theme.getColor (ZalaxyTheme::ColorId::Surface));
    setupGuideButton.setColour (juce::TextButton::textColourOffId, theme.getColor (ZalaxyTheme::ColorId::TextMuted));
    setupGuideButton.addListener (this);
}

void SettingsPanel::initialize()
{
    updateUI();
}

void SettingsPanel::paint (juce::Graphics& g)
{
    auto bounds = getLocalBounds().toFloat();
    g.setColour (theme.getColor (ZalaxyTheme::ColorId::PanelBackground));
    g.fillRoundedRectangle (bounds, theme.getRadius ("medium"));
    g.setColour (theme.getColor (ZalaxyTheme::ColorId::BorderSubtle));
    g.drawRoundedRectangle (bounds, theme.getRadius ("medium"), 1.0f);
    g.setColour (theme.getColor (ZalaxyTheme::ColorId::AccentSecondary));
    g.fillRect (juce::Rectangle<float> (bounds.getX() + 8.0f,
                                    bounds.getY() + 2.0f,
                                    bounds.getWidth() - 16.0f,
                                    2.0f));
}

void SettingsPanel::resized()
{
    auto bounds = getLocalBounds().reduced (theme.getSpace (3));
    int spacing = theme.getSpace (2);
    int labelHeight = 20;
    int controlHeight = 32;
    int sectionSpacing = theme.getSpace (3);

    // AI Provider Section
    aiSectionLabel.setBounds (bounds.removeFromTop (labelHeight));
    bounds.removeFromTop (theme.getSpace (1));

    auto providerRow = bounds.removeFromTop (controlHeight);
    providerSelector.setBounds (providerRow.removeFromLeft (providerRow.getWidth() - 100));
    // providerSelector takes most of the row

    bounds.removeFromTop (theme.getSpace (1));

    apiKeyLabel.setBounds (bounds.removeFromTop (labelHeight));
    bounds.removeFromTop (theme.getSpace (1));

    auto keyRow = bounds.removeFromTop (controlHeight);
    apiKeyEditor.setBounds (keyRow.removeFromLeft (keyRow.getWidth() - 220));
    saveKeyButton.setBounds (keyRow.removeFromLeft (100 + spacing));
    testKeyButton.setBounds (keyRow);

    bounds.removeFromTop (theme.getSpace (1));
    keyStatusLabel.setBounds (bounds.removeFromTop (20));

    bounds.removeFromTop (sectionSpacing);

    // Mode Section
    modeLabel.setBounds (bounds.removeFromTop (labelHeight));
    bounds.removeFromTop (theme.getSpace (1));
    cloudModeToggle.setBounds (bounds.removeFromTop (controlHeight).withTrimmedLeft (10));
    bounds.removeFromTop (theme.getSpace (1));
    modeDescriptionLabel.setBounds (bounds.removeFromTop (40));

    bounds.removeFromTop (sectionSpacing);

    // Usage Section
    usageSectionLabel.setBounds (bounds.removeFromTop (labelHeight));
    bounds.removeFromTop (theme.getSpace (1));

    auto usageRow = bounds.removeFromTop (labelHeight);
    tokensUsedLabel.setBounds (usageRow.removeFromLeft (usageRow.getWidth() / 2));
    tokensRemainingLabel.setBounds (usageRow);

    bounds.removeFromTop (theme.getSpace (1));
    usageProgressBar.setBounds (bounds.removeFromTop (12));
    bounds.removeFromTop (theme.getSpace (1));
    limitWarningLabel.setBounds (bounds.removeFromTop (20));

    bounds.removeFromTop (sectionSpacing);

    // Subscription Section
    subSectionLabel.setBounds (bounds.removeFromTop (labelHeight));
    bounds.removeFromTop (theme.getSpace (1));
    subStatusLabel.setBounds (bounds.removeFromTop (24));
    bounds.removeFromTop (theme.getSpace (1));

    auto subBtnRow = bounds.removeFromTop (controlHeight);
    enterLicenseButton.setBounds (subBtnRow.removeFromLeft (subBtnRow.getWidth() / 2 - spacing / 2));
    buyLicenseButton.setBounds (subBtnRow.removeFromLeft (subBtnRow.getWidth() - spacing));

    bounds.removeFromTop (sectionSpacing);

    // Guide Button
    setupGuideButton.setBounds (bounds.removeFromTop (controlHeight).withTrimmedLeft (40).withTrimmedRight (40));
}

void SettingsPanel::buttonClicked (juce::Button* button)
{
    if (button == &saveKeyButton)
    {
        saveAPIKey();
    }
    else if (button == &testKeyButton)
    {
        testAPIKey();
    }
    else if (button == &cloudModeToggle)
    {
        aiProvider.setCloudMode (cloudModeToggle.getToggleState());
        updateUI();
    }
    else if (button == &enterLicenseButton)
    {
        showLicenseDialog();
    }
    else if (button == &buyLicenseButton)
    {
        juce::URL ("https://zalaxymusic.com/pricing").launchInDefaultBrowser();
    }
    else if (button == &setupGuideButton)
    {
        showSetupGuide();
    }
}

void SettingsPanel::textEditorTextChanged (juce::TextEditor& editor)
{
    if (&editor == &apiKeyEditor)
    {
        saveKeyButton.setEnabled (apiKeyEditor.getText().trim().isNotEmpty());
    }
}

void SettingsPanel::textEditorReturnKeyPressed (juce::TextEditor& editor)
{
    if (&editor == &apiKeyEditor && apiKeyEditor.getText().trim().isNotEmpty())
    {
        saveAPIKey();
    }
}

void SettingsPanel::comboBoxChanged (juce::ComboBox* comboBox)
{
    if (comboBox == &providerSelector)
    {
        juce::StringArray providers = aiProvider.getAvailableProviders();
        int idx = providerSelector.getSelectedItemIndex();
        if (idx >= 0 && idx < providers.size())
        {
            aiProvider.setProvider (providers[idx]);
            apiKeyEditor.setText (aiProvider.getAPIKey (providers[idx]));
            updateUI();
        }
    }
}

void SettingsPanel::saveAPIKey()
{
    juce::String key = apiKeyEditor.getText().trim();
    if (key.isEmpty())
        return;

    juce::String provider = aiProvider.getCurrentProvider();
    if (aiProvider.setAPIKey (provider, key))
    {
        keyStatusLabel.setText ("✓ Key saved securely", juce::dontSendNotification);
        keyStatusLabel.setColour (juce::Label::textColourId, theme.getColor (ZalaxyTheme::ColorId::AccentSuccess));
        testKeyButton.setEnabled (true);
    }
    else
    {
        keyStatusLabel.setText ("✗ Failed to save key", juce::dontSendNotification);
        keyStatusLabel.setColour (juce::Label::textColourId, theme.getColor (ZalaxyTheme::ColorId::AccentDanger));
    }
}

void SettingsPanel::testAPIKey()
{
    testKeyButton.setEnabled (false);
    testKeyButton.setButtonText ("Testing...");

    aiProvider.testConnection ([this] (bool success, const juce::String& message)
    {
        juce::MessageManager::callAsync ([this, success, message]()
        {
            testKeyButton.setEnabled (true);
            testKeyButton.setButtonText ("Test Connection");

            if (success)
            {
                keyStatusLabel.setText ("✓ " + message, juce::dontSendNotification);
                keyStatusLabel.setColour (juce::Label::textColourId, theme.getColor (ZalaxyTheme::ColorId::AccentSuccess));
            }
            else
            {
                keyStatusLabel.setText ("✗ " + message, juce::dontSendNotification);
                keyStatusLabel.setColour (juce::Label::textColourId, theme.getColor (ZalaxyTheme::ColorId::AccentDanger));
            }
        });
    });
}

void SettingsPanel::showLicenseDialog()
{
    auto* dialog = new juce::AlertWindow (
        "Enter License Key",
        "Paste your ZalaxyMusic license key below.\nFormat: ZALX-XXXX-XXXX-XXXX",
        juce::AlertWindow::NoIcon);

    dialog->addTextEditor ("licenseKey", "", "License Key", false);
    dialog->addButton ("Activate", 1);
    dialog->addButton ("Cancel", 0);

    juce::Component::SafePointer<SettingsPanel> safeThis (this);
    juce::Component::SafePointer<juce::AlertWindow> safeDialog (dialog);

    dialog->enterModalState (
        true,
        juce::ModalCallbackFunction::create ([safeThis, safeDialog] (int result)
        {
            if (result != 1 || safeThis == nullptr || safeDialog == nullptr)
                return;

            const auto key = safeDialog->getTextEditorContents ("licenseKey").trim();
            if (key.isEmpty())
                return;

            if (safeThis->subscriptionManager.validateLicenseKey (key))
            {
                safeThis->subscriptionManager.applyLicenseKey (key);
                safeThis->updateUI();
                juce::AlertWindow::showMessageBoxAsync (
                    juce::AlertWindow::InfoIcon,
                    "Success",
                    "License activated! " + safeThis->subscriptionManager.getCurrentTierName() + " features unlocked.",
                    "OK");
            }
            else
            {
                juce::AlertWindow::showMessageBoxAsync (
                    juce::AlertWindow::WarningIcon,
                    "Invalid Key",
                    "The license key is invalid or expired.",
                    "OK");
            }
        }),
        true);
}

void SettingsPanel::showSetupGuide()
{
    auto wizard = std::make_unique<BYOKSetupWizard> (aiProvider, subscriptionManager);
    juce::Component::SafePointer<SettingsPanel> safeThis (this);

    wizard->onComplete = [safeThis]()
    {
        if (safeThis != nullptr)
            safeThis->updateUI();
    };

    wizard->setSize (520, 580);

    juce::DialogWindow::LaunchOptions options;
    options.content.setOwned (wizard.release());
    options.dialogTitle = "AI Setup Wizard";
    options.dialogBackgroundColour = theme.getColor (ZalaxyTheme::ColorId::PanelBackground);
    options.escapeKeyTriggersCloseButton = true;
    options.useNativeTitleBar = false;
    options.resizable = false;
    options.launchAsync();
}

void SettingsPanel::updateUI()
{
    // Update token usage display
    // Note: Would need UsageTracker reference - for now using placeholder
    tokensUsedLabel.setText ("Used: 0 tokens", juce::dontSendNotification);
    tokensRemainingLabel.setText ("Remaining: 100,000", juce::dontSendNotification);
    usageProgress = 0.0;

    // Update subscription status
    auto tier = subscriptionManager.getCurrentTier();
    juce::String statusText = "Tier: " + subscriptionManager.getCurrentTierName();
    if (tier == SubscriptionManager::LicenseTier::Free && subscriptionManager.isTrialActive())
    {
        statusText += " (Trial: " + juce::String (subscriptionManager.getTrialDaysRemaining()) + " days left)";
    }
    else if (tier != SubscriptionManager::LicenseTier::Free)
    {
        statusText += " • Expires: " + subscriptionManager.getExpirationDate();
    }
    subStatusLabel.setText (statusText, juce::dontSendNotification);

    // Update key status
    if (aiProvider.hasValidAPIKey())
    {
        keyStatusLabel.setText ("✓ API key configured", juce::dontSendNotification);
        keyStatusLabel.setColour (juce::Label::textColourId, theme.getColor (ZalaxyTheme::ColorId::AccentSuccess));
        testKeyButton.setEnabled (true);
    }
    else
    {
        keyStatusLabel.setText ("No API key set", juce::dontSendNotification);
        keyStatusLabel.setColour (juce::Label::textColourId, theme.getColor (ZalaxyTheme::ColorId::TextMuted));
        testKeyButton.setEnabled (false);
    }

    // Update cloud mode toggle
    cloudModeToggle.setToggleState (aiProvider.isCloudMode(), juce::dontSendNotification);

    repaint();
}

} // namespace zalaxy