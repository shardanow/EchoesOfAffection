# ? Quick Fix - Blueprint Interface Errors

## ?? Problem
```
Assertion failed: "Do not directly call Event functions in Interfaces"
```

## ? Solution (30 seconds)

### In your Blueprint:

**Replace ALL these nodes:**

| ? OLD (Error) | ? NEW (Works) |
|----------------|----------------|
| `Get Current Time` | `Get Current Time (BP)` |
| `Get Current Phase` | `Get Current Phase (BP)` |
| `Get Current Season` | `Get Current Season (BP)` |
| `Get Normalized Time` | `Get Normalized Time (BP)` |
| `Get Time Scale` | `Get Time Scale (BP)` |
| `Is Time Paused` | `Is Time Paused (BP)` |
| `Set Current Time` | `Set Current Time (BP)` |
| `Set Time Scale` | `Set Time Scale (BP)` |
| `Advance Time` | `Advance Time (BP)` |
| `Pause Time` | `Pause Time (BP)` |
| `Resume Time` | `Resume Time (BP)` |
| `Register Listener` | `Register Listener (BP)` |
| `Unregister Listener` | `Unregister Listener (BP)` |

## ?? How to Find BP Nodes

1. Right-click in Blueprint
2. Type: `BP Get Current` (with BP prefix!)
3. Select node with `(BP)` suffix

## ?? Example

### Before (Error):
```
Get Time of Day Subsystem
    ?
    ?
Get Current Phase  ? ? ERROR!
```

### After (Fixed):
```
Get Time of Day Subsystem
    ?
    ?
Get Current Phase (BP)  ? ? WORKS!
```

## ?? Alternative: Use Component

Instead of calling methods, add **Time Of Day Listener Component**:

1. Add Component ? `Time Of Day Listener Component`
2. Set event flags (Receive Phase Events, etc.)
3. Component auto-registers on BeginPlay
4. ? Done!

## ?? More Info

- **Full Guide**: `BLUEPRINT_ERROR_FIX.md`
- **Quick Start**: `QUICKSTART.md`
- **Examples**: `CONTENT_GUIDE.md`

---

**Remember**: Always use `(BP)` versions in Blueprint! ??
