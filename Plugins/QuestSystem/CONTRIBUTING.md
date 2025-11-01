# Contributing to Quest System

Thank you for your interest in contributing! This document provides guidelines for contributing to the Quest System plugin.

---

## Table of Contents

- [Code of Conduct](#code-of-conduct)
- [Getting Started](#getting-started)
- [Development Process](#development-process)
- [Coding Standards](#coding-standards)
- [Testing Guidelines](#testing-guidelines)
- [Documentation](#documentation)
- [Pull Request Process](#pull-request-process)

---

## Code of Conduct

### Our Pledge

We pledge to make participation in our project a harassment-free experience for everyone.

### Our Standards

**Positive Behavior:**
- ? Using welcoming and inclusive language
- ? Being respectful of differing viewpoints
- ? Gracefully accepting constructive criticism
- ? Focusing on what is best for the community
- ? Showing empathy towards others

**Unacceptable Behavior:**
- ? Trolling, insulting/derogatory comments
- ? Public or private harassment
- ? Publishing others' private information
- ? Other conduct which could be considered inappropriate

---

## Getting Started

### Prerequisites

- Unreal Engine 5.4+ installed
- Visual Studio 2022 or Rider
- Git for version control
- Basic knowledge of C++ and Unreal Engine

### Setting Up Development Environment

1. **Clone Repository**
   ```bash
   git clone https://github.com/shardanow/EchoesOfAffection.git
   cd EchoesOfAffection
   ```

2. **Generate Project Files**
   ```bash
   Right-click EchoesOfAffection.uproject
   Select "Generate Visual Studio project files"
   ```

3. **Open in IDE**
   ```bash
   Open EchoesOfAffection.sln in Visual Studio
 ```

4. **Build**
   ```bash
   Build Solution (Ctrl+Shift+B)
   ```

### Project Structure

```
Plugins/QuestSystem/
?? Source/
?  ?? QuestSystemRuntime/    (Runtime code)
?  ?? QuestSystemEditor/     (Editor-only code)
?? Config/      (Configuration)
?? Content/    (Example content)
?? Docs/  (Documentation)
?? Resources/      (Icons, assets)
```

---

## Development Process

### Branching Strategy

- `master` - Stable release branch
- `develop` - Main development branch
- `feature/[name]` - New features
- `bugfix/[name]` - Bug fixes
- `hotfix/[name]` - Critical fixes

### Workflow

1. **Create Branch**
   ```bash
   git checkout develop
   git pull origin develop
   git checkout -b feature/my-new-feature
   ```

2. **Make Changes**
   - Write code following standards
   - Add/update tests
   - Update documentation

3. **Commit**
   ```bash
   git add .
   git commit -m "feat: add new objective type"
   ```

4. **Push**
   ```bash
   git push origin feature/my-new-feature
   ```

5. **Create Pull Request**
   - Open PR on GitHub
   - Fill in description template
   - Link related issues

---

## Coding Standards

### C++ Style Guide

**General Rules:**
- Follow Unreal Engine coding standards
- Use `U` prefix for UObject-derived classes
- Use `F` prefix for structs
- Use `E` prefix for enums
- Use `I` prefix for interfaces

**Naming:**
```cpp
// ? Good
class UQuestSubsystem : public UGameInstanceSubsystem { };
struct FQuestSaveData { };
enum class EQuestState : uint8 { };

// ? Bad
class QuestSubsystem { };  // Missing U prefix
struct questData { };      // Wrong casing
enum QuestState { };      // Should be enum class
```

**Formatting:**
```cpp
// ? Good
bool UQuestSubsystem::StartQuest(FName QuestId)
{
    if (QuestId.IsNone())
    {
        return false;
    }
    
  // Implementation
    return true;
}

// ? Bad
bool UQuestSubsystem::StartQuest(FName QuestId){
if(QuestId.IsNone()) return false;
// Implementation
return true;}
```

**Comments:**
```cpp
/**
 * Starts a quest by ID
 * 
 * @param QuestId Unique quest identifier
 * @return true if quest started successfully
 */
UFUNCTION(BlueprintCallable, Category = "Quest")
bool StartQuest(FName QuestId);
```

### Blueprint Exposure

**Always expose to Blueprint when:**
- Function is useful for designers
- Function has no side effects
- Function is player-facing

**Blueprint Metadata:**
```cpp
UFUNCTION(BlueprintCallable, Category = "Quest", 
    meta = (WorldContext = "WorldContextObject", 
DisplayName = "Start Quest",
            Keywords = "begin activate quest"))
static bool StartQuest(UObject* WorldContextObject, FName QuestId);
```

### Performance Guidelines

**Do:**
- ? Cache references when possible
- ? Use `const&` for large parameters
- ? Avoid allocations in hot paths
- ? Use `TObjectPtr` for UE5.1+
- ? Profile before optimizing

**Don't:**
- ? Use Tick unless absolutely necessary
- ? String comparisons in tight loops
- ? Synchronous asset loading in game thread
- ? Deep recursion
- ? Premature optimization

---

## Testing Guidelines

### Unit Tests

Create tests for new functionality:

```cpp
#include "Misc/AutomationTest.h"
#include "QuestSubsystem.h"

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
 FQuestSystemStartQuestTest,
    "QuestSystem.Core.StartQuest",
    EAutomationTestFlags::ApplicationContextMask | 
    EAutomationTestFlags::ProductFilter
)

bool FQuestSystemStartQuestTest::RunTest(const FString& Parameters)
{
    // Setup
    UQuestSubsystem* QuestSystem = GetQuestSubsystem();
    
    // Test
    bool bStarted = QuestSystem->StartQuest(FName("Q_Test"));
    
    // Verify
    TestTrue("Quest started", bStarted);
    TestEqual("Quest is active", 
        QuestSystem->GetQuestState(FName("Q_Test")), 
      EQuestState::Active);

    return true;
}
```

### Manual Testing Checklist

Before submitting PR:

- [ ] Code compiles without warnings
- [ ] All unit tests pass
- [ ] Manual testing completed
- [ ] No console errors/warnings
- [ ] Works in packaged build
- [ ] Documentation updated
- [ ] Examples still work

---

## Documentation

### Code Documentation

**Header Files:**
```cpp
/**
 * Quest Subsystem - Main quest management system
 * 
 * Handles quest lifecycle, progress tracking, and event routing.
 * Event-driven architecture with zero per-frame overhead.
 * 
 * @see UQuestAsset for quest definition
 * @see UQuestEventBus for event system
 */
UCLASS()
class QUESTSYSTEMRUNTIME_API UQuestSubsystem : public UGameInstanceSubsystem
{
    // ...
};
```

**Functions:**
```cpp
/**
 * Starts a quest if dependencies are met
 * 
 * Checks all quest dependencies before starting.
 * Emits OnQuestStarted event if successful.
 * 
 * @param QuestId Unique quest identifier
 * @return true if quest started, false if dependencies not met or already active
 * 
 * @note Quest asset is loaded synchronously - consider async for production
 * @warning Starting same quest twice will return false
 */
UFUNCTION(BlueprintCallable, Category = "Quest")
bool StartQuest(FName QuestId);
```

### User Documentation

When adding features, update:
- README.md (if major feature)
- API_Reference.md (all new APIs)
- Examples.md (if example-worthy)
- CheatSheet.md (if commonly used)

Use clear, beginner-friendly language.

---

## Pull Request Process

### PR Template

```markdown
## Description
Brief description of changes

## Type of Change
- [ ] Bug fix
- [ ] New feature
- [ ] Breaking change
- [ ] Documentation update

## Testing
- [ ] Unit tests added/updated
- [ ] Manual testing completed
- [ ] All tests pass

## Checklist
- [ ] Code follows style guidelines
- [ ] Self-review completed
- [ ] Comments added for complex code
- [ ] Documentation updated
- [ ] No new warnings
- [ ] Backward compatible (or documented otherwise)
```

### Review Process

1. **Automated Checks**
   - Code compiles
   - Tests pass
   - No style violations

2. **Code Review**
   - At least one reviewer approval
   - Address all feedback
   - No unresolved conversations

3. **Documentation Review**
   - API docs updated
   - Examples provided
   - CHANGELOG updated

4. **Final Approval**
   - Maintainer approval
   - CI/CD pipeline passes
   - Merge to target branch

### Commit Message Format

Use Conventional Commits:

```
<type>[optional scope]: <description>

[optional body]

[optional footer(s)]
```

**Types:**
- `feat`: New feature
- `fix`: Bug fix
- `docs`: Documentation changes
- `style`: Code style changes (formatting)
- `refactor`: Code refactoring
- `perf`: Performance improvements
- `test`: Adding tests
- `chore`: Build/tooling changes

**Examples:**
```bash
feat(core): add quest template system
fix(editor): resolve validation crash on empty quest
docs(api): update StartQuest documentation
refactor(subsystem): split large class into smaller components
perf(eventbus): optimize event routing for 100+ subscribers
```

---

## Feature Requests

### How to Request

1. **Check Existing**
 - Search GitHub Issues
   - Check [TODO.md](TODO.md)

2. **Create Issue**
   - Use feature request template
   - Describe use case clearly
   - Provide examples if possible

3. **Discussion**
   - Engage in discussion
   - Be open to feedback
 - Consider alternatives

### Good Feature Requests

? **Good:**
```
Title: Add visual graph editor for quest flow

Description:
Currently, quest creation is done via data assets which can be 
tedious for complex quest chains. A visual graph editor would allow
designers to:

1. Visually see quest flow
2. Create branches easily
3. Validate dependencies visually

Example from other tools: [Screenshot]

Use cases:
- Complex branching quests
- Quest chain visualization
- Faster iteration for designers

Implementation ideas:
- Use Slate graph framework
- Similar to Blueprint editor
- Save/load from existing QuestAsset format
```

? **Bad:**
```
Add graph editor plz
```

---

## Bug Reports

### How to Report

1. **Search First**
   - Check existing issues
   - Might already be reported/fixed

2. **Provide Details**
   - Engine version
   - Plugin version
   - Steps to reproduce
 - Expected vs actual behavior
   - Screenshots/logs

3. **Minimal Reproduction**
   - Simplest case that shows bug
 - Remove unrelated code
   - Provide example project if possible

### Good Bug Reports

? **Good:**
```
Title: Quest fails to save when objective has special characters

Environment:
- UE 5.4.3
- Quest System v1.0.0
- Windows 11

Steps to Reproduce:
1. Create quest with ID "Q_Test"
2. Add objective with title "Collect "Special" Items"
3. Start quest
4. Call SaveQuestProgress()
5. Load save game

Expected: Progress saves correctly
Actual: Save fails with error "Invalid character in JSON"

Logs:
[Error] QuestSubsystem: Failed to serialize quest Q_Test
[Error] SaveGame: JSON serialization failed

Screenshots: [attached]
```

? **Bad:**
```
Save doesn't work
```

---

## Questions & Support

### Before Asking

1. **Check Documentation**
   - README.md
   - API_Reference.md
   - Examples.md

2. **Search Issues**
   - Might already be answered

3. **Try Example Project**
   - See if examples work

### Where to Ask

- **General Questions**: GitHub Discussions
- **Bug Reports**: GitHub Issues
- **Feature Requests**: GitHub Issues
- **Security Issues**: Email (private)
- **Chat**: Discord (TBD)

---

## Recognition

Contributors will be:
- Listed in CHANGELOG.md
- Credited in documentation
- Mentioned in release notes
- Given contributor badge (if applicable)

Thank you for contributing! ??

---

## License

By contributing, you agree that your contributions will be licensed under the same license as the project.

---

## Contact

- GitHub: [@shardanow](https://github.com/shardanow)
- Project: [EchoesOfAffection](https://github.com/shardanow/EchoesOfAffection)
- Issues: [GitHub Issues](https://github.com/shardanow/EchoesOfAffection/issues)
