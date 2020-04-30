Galiceanu Alina-Madalina 322CA, Tema2 PC


Clientul este implementat in fisierul "subscriber.cpp".
In implementarea acestuia am urmat pasii prezentati in cadrul
laboratorului. Subscrierul poate interactiona cu utilizatorul 
prin primirea de input de la tastatura a unor comenzi precum
"exit", "subscribe", "unsubscribe". Aceastea avand functionalitatea
prezentata in enunt. 

In ceea ce priveste serverul (server.cpp), acesta primeste topicuri de 
la clientul UDP. Am parsat aceste topicuri respectand indicatiile din enunt
pentru fiecare tip de date prezent in topicul curent.
Asemenea cliantului serverul intreactioneaza cu utilizatorul, primiind 
comanda de exit, ce inchide automat toti clientii si serverul.

Tema nu este completa, urmatorii pasii ar fi fost trimiterea id ului
fiecarui client nou conectat, primirea acestuia de catre server. 
Serveru verifica daca id este deja disponibil, cazul in care nu este
trimite un mesaj clientului si inchide conexiunea acestuia. In client valid
asteapta comezi de la tastatura de subscribe sau unsubscribe. Comanda primita
urmeaza sa fie trimisa serverului iar acesta va memora clientul intr o multime
ce va retine clientii ambonati la fiecare topic si va trimite inapoi mesaje 
pe baza topicului primit de la client.
In cazul comenzii unsubscribe clientul este scos din multimea ce retine 
clientii abonati la fiecare topic, daca acesta exista.

