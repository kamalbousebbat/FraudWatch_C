
# Compilation
Write-Host "Compilation de TaskFlow..." -ForegroundColor Cyan

gcc -Wall -Wextra -Wpedantic -std=c11 src/main.c src/execution.c src/machine_manager.c src/process_manager.c -o TaskFlow.exe

# Vérification de la compilation
if ($LASTEXITCODE -eq 0) {
    Write-Host "Compilation reussie !" -ForegroundColor Green

    # Execution
    Write-Host "Lancement de TaskFlow..." -ForegroundColor Cyan
    .\TaskFlow.exe
}
else {
    Write-Host "Erreur lors de la compilation !" -ForegroundColor Red
}
