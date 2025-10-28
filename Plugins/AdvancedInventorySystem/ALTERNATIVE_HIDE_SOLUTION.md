# Alternative Implementation - Hide Instead of Remove

If widgets are losing their bindings after `RemoveFromParent()`, you can change the approach to **hide** the widget instead of removing it:

## Change HideInventory():

```cpp
void UInventoryWidgetBase::HideInventory()
{
    // Hide tooltip first
    HideItemTooltip();

    // Instead of RemoveFromParent(), just hide the widget
    SetVisibility(ESlateVisibility::Collapsed);

    // Restore game input mode
  APlayerController* PC = GetOwningPlayer();
    if (PC)
    {
        FInputModeGameOnly InputMode;
        PC->SetInputMode(InputMode);
        PC->bShowMouseCursor = false;
    }
}
```

## Change ShowInventory():

```cpp
void UInventoryWidgetBase::ShowInventory()
{
    // Only add to viewport ONCE (first time)
    if (!IsInViewport())
    {
      AddToViewport(10);
    }

    // Show the widget
    SetVisibility(ESlateVisibility::Visible);

    // ALWAYS refresh when showing
    RefreshInventorySlots();
    UpdateWidgets();

 // Set input mode
    // ...
}
```

## Benefits:

- ? Widget stays in viewport (never destroyed)
- ? All bindings remain intact
- ? No `NativeConstruct()` called multiple times
- ? Performance: No widget recreation

## Drawbacks:

- Widget always in memory (minimal impact)
- Takes up viewport slot (not an issue)
