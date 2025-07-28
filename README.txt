Othello Malware é um programa simples que usa engenharia social para carregar e executar um shellcode na memória.

--- Como funciona? ---

1. O programa mata alguns processos do sistema (por padrão, navegadores como o Google Chrome) e finge ser o antivírus do Windows.
2. Espera que o usuário acredite na mensagem, reinicie a máquina e desative o antivírus.
3. Após um tempo (3 minutos, por padrão), na esperança de que o antivírus foi desativado, o programa tenta carregar e executar um shellcode malicioso na memória.

--- Ambiente compatível ---
1. O programa, por padrão, funciona apenas em máquinas Windows. Foi testado no Windows 10 x64 com Microsoft Defender.

--- Vestígios deixados na máquina ---

1. Uma chave de registro é criada para permitir que o malware (othello.exe) seja executado automaticamente na inicialização.
2. Um arquivo DAT (C:/Users/Public/othelloruntime.dat) é usado para verificar se a máquina foi reiniciada desde a última execução do programa.

--- Arquivos ---

./sh3llc0ode.h: header simples contendo o shellcode executado pelo programa final.
./othello.c: código C principal.
./Makefile: arquivo usado para compilar o programa (testado em WSL do Kali Linux).
