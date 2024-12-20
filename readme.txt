Turno: RC11L04 
Grupo: 19

Trabalhado realizado por:
 - Tiago Castro Santos (106794)
 - João Ricardo Fernades Caçador (107301)

A pasta src é composta por três subpastas. Uma pasta server, uma pasta player e uma pasta common que possui código útil a ambas as
aplicações (leitura/verificação de inputs, entre outros).

A pasta reports possui os relatórios (em formato HTML) dos testes efetuados. 

Para correr a aplicação basta, compilar os executáveis usando o comando "make" nesta diretoria,
de seguida basta correr o servidor usando o comando "./GS", e por fim correr o cliente usando o comando "./player".

Observações:
 - Os ficheiros recebido como resposta aos comandos show_trials e scoreboard são guardados na pasta src/player/scores, 
 em relação à diretoria atual.
 - Todos os paths são tratados de forma dinâmica em relação à diretoria principal, portanto se as pastas
server e player forem mudadas de localização, estes paths terão de ser alterados.

Boas Festas
