# ATmega128RFA1-AT86RF231-Reader-Firmware
Our startup company provides an advanced system for Operators in the Commercial Laundry Industry to replace coins and smart cards using an online solution to manage their business, primarily by automating the collection of revenue, reducing service cost and having a user create a prepaid laundry account. 
A micro-controller, ATmega128RF including a radio chip AT86RF231 transmits the data that came from the RF card to identify the user. This data is used to access the user account to determine if there is sufficient funds to satisfy the vend.  The RF reader is attached to each laundry machine controller (or dryer) to enable users to operate the machine from a prepaid laundry account. Readers communicate to a secure wireless controller which in turn transmit each transaction to the Pinpoint application in the cloud.
The reader firmware performs the following steps:

1. It sends a command to the laundry machine controller to enquire what machine type it is?
  The machine will be either a washer 331 or dryer 411.
2. Then through a coordinator the reader communicates with the server to get the setup string for the machine type.
3. The setup string is saved to the reader's flash memory (non volatile area).
4. The vend price is parsed out of the setup string and displayed to the washing machine display.
5. The reader sends out beacons to the coordinator asking: "I am here, are you still here? When there is no acknowledgment from the coordinator then you are in offline mode go to step 15.
6. The reader is scanning for a card wave.
7. Does the reader detects a card wave it parses out the serial number.
8. The reader confirms to the coordinator, that it received a serial number and asked the coordinator to send it on to the server
9. The reader is now waiting for the server's response, that there is enough money in the account.
10. The reader sends a command to the washing machine to display the account balance.
11. The reader is listening, that the user has pressed the Start button on the machine.
12. If start button is pressed the reader sends the request to the coordinator, then to the server.
13. The server responds and sends a request to the machine controller to start the machine.
14. The reader is waiting for the end of the washing cycle. 
15. You are in offline mode. As soon the coordinator is back online transactions are reconciled with the server.




