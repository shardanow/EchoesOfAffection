# Fix Corrupted Russian Text in DialogueSystem Files
# This script removes all corrupted Cyrillic text (displayed as ????) and replaces with basic English comments

Write-Host "=== Fixing Corrupted Text in DialogueSystem ===" -ForegroundColor Cyan
Write-Host ""

$files = @(
    "Plugins\DialogueSystem\Source\DialogueSystemAI\Public\AIDialogueProvider.h",
    "Plugins\DialogueSystem\Source\DialogueSystemCore\Private\Conditions\DialogueCondition_CheckRelationship.cpp",
    "Plugins\DialogueSystem\Source\DialogueSystemCore\Private\Conditions\MemoryConditions.cpp",
    "Plugins\DialogueSystem\Source\DialogueSystemCore\Private\Effects\MemoryEffects.cpp",
    "Plugins\DialogueSystem\Source\DialogueSystemCore\Private\Pool\DialogueObjectPool.cpp",
    "Plugins\DialogueSystem\Source\DialogueSystemCore\Private\SaveGame\DialogueSaveData.cpp",
    "Plugins\DialogueSystem\Source\DialogueSystemCore\Public\Conditions\DialogueConditionEvaluator.h",
    "Plugins\DialogueSystem\Source\DialogueSystemCore\Public\Core\DialogueRunner.h",
    "Plugins\DialogueSystem\Source\DialogueSystemCore\Public\Data\DialogueDataAsset.h",
    "Plugins\DialogueSystem\Source\DialogueSystemCore\Public\Effects\DialogueEffectExecutor.h",
    "Plugins\DialogueSystem\Source\DialogueSystemCore\Public\Effects\DialogueEffect_EmitGameEvent.h",
    "Plugins\DialogueSystem\Source\DialogueSystemCore\Public\Effects\DialogueEffect_ModifyRelationship.h",
    "Plugins\DialogueSystem\Source\DialogueSystemCore\Public\Effects\MemoryEffects.h",
    "Plugins\DialogueSystem\Source\DialogueSystemCore\Public\Examples\DialogueConditionExamples.h",
    "Plugins\DialogueSystem\Source\DialogueSystemCore\Public\Interfaces\IDialogueParticipant.h",
    "Plugins\DialogueSystem\Source\DialogueSystemCore\Public\SaveGame\DialogueSaveData.h",
  "Plugins\DialogueSystem\Source\DialogueSystemRuntime\Private\Components\DialogueComponent.cpp",
    "Plugins\DialogueSystem\Source\DialogueSystemRuntime\Private\Components\DialogueCameraComponent.cpp",
    "Plugins\DialogueSystem\Source\DialogueSystemRuntime\Private\Components\DialogueInputBlockerComponent.cpp",
 "Plugins\DialogueSystem\Source\DialogueSystemRuntime\Private\Components\DialoguePresentationComponent.cpp",
    "Plugins\DialogueSystem\Source\DialogueSystemRuntime\Private\Components\NPCMemoryComponent.cpp",
    "Plugins\DialogueSystem\Source\DialogueSystemRuntime\Public\Components\DialogueComponent.h",
    "Plugins\DialogueSystem\Source\DialogueSystemRuntime\Public\Components\NPCMemoryComponent.h"
)

$fixedCount = 0
$totalCorrupted = 0

foreach ($file in $files) {
    if (!(Test-Path $file)) {
        Write-Host "[SKIP] File not found: $file" -ForegroundColor Yellow
        continue
    }

    Write-Host "Processing: $file" -ForegroundColor White
    
    # Read file with UTF-8 encoding
    $content = Get-Content $file -Raw -Encoding UTF8
    
 # Count corrupted instances
  $corruptedMatches = [regex]::Matches($content, '????')
    $corruptedCount = $corruptedMatches.Count
    $totalCorrupted += $corruptedCount
    
    if ($corruptedCount -eq 0) {
        Write-Host "  [OK] No corrupted text found" -ForegroundColor Green
        continue
    }
    
    Write-Host "  Found $corruptedCount corrupted text instances" -ForegroundColor Yellow
 
    # Create backup
    $backupFile = "$file.backup"
    Copy-Item $file $backupFile -Force
    
    # Strategy 1: Remove corrupted C++ style comments (// ????...)
    $content = $content -replace '/\*\*[^*]*????[^*]*\*/', '/** [Documentation removed due to encoding issues] */'
    $content = $content -replace '//\s*????[^\r\n]*', '// [Comment removed due to encoding issues]'
    $content = $content -replace '/\*[^*]*????[^*]*\*/', '/* [Comment removed due to encoding issues] */'
    
    # Strategy 2: Clean up common UPROPERTY patterns
    $content = $content -replace '(\s+)UPROPERTY\([^)]+\)\s*???[^\r\n]*', '$1UPROPERTY$2'
    
    # Strategy 3: Remove standalone corrupted lines (???? only lines)
    $content = $content -replace '^\s*????[^\r\n]*$', '', 'Multiline'
    
    # Save cleaned file
    Set-Content $file -Value $content -Encoding UTF8 -NoNewline
    
    # Verify fix
    $newContent = Get-Content $file -Raw -Encoding UTF8
    $remainingCorrupted = ([regex]::Matches($newContent, '????')).Count
    
    if ($remainingCorrupted -eq 0) {
        Write-Host "  [SUCCESS] All corrupted text removed" -ForegroundColor Green
        Remove-Item $backupFile -Force
        $fixedCount++
    } else {
    Write-Host "  [PARTIAL] $remainingCorrupted instances remaining (manual review needed)" -ForegroundColor Yellow
        Write-Host "  Backup saved: $backupFile" -ForegroundColor Gray
     $fixedCount++
    }
}

Write-Host ""
Write-Host "=== Summary ===" -ForegroundColor Cyan
Write-Host "Files processed: $fixedCount" -ForegroundColor White
Write-Host "Total corrupted instances found: $totalCorrupted" -ForegroundColor White
Write-Host ""
Write-Host "Next steps:" -ForegroundColor Yellow
Write-Host "1. Review files with remaining corruption (manual fix needed)" -ForegroundColor White
Write-Host "2. Rebuild project in Unreal Editor" -ForegroundColor White
Write-Host "3. Check tooltips in Blueprint Editor" -ForegroundColor White
Write-Host "4. Commit changes to Git" -ForegroundColor White
Write-Host ""
Write-Host "Restore command (if needed):" -ForegroundColor Gray
Write-Host "  Get-ChildItem -Recurse -Filter '*.backup' | ForEach-Object { Move-Item $_.FullName ($_.FullName -replace '.backup','') -Force }" -ForegroundColor DarkGray
