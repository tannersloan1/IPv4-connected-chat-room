I use Microsoft Visual Studio, so this explanation will follow that program. However, I assume it would still work with a different way to run and compile the code.

These programs facilitate a chat window between two different machines over an IPv4 connection. How to use/run the programs is below:

1. Download both the client.c and server.c files from the folder.
2. Open (or download at "https://visualstudio.microsoft.com/") Microsoft Visual Studio on both windows computers.
3. Now you need to create an empty c++ project, for me it is the top option when I select create new project. Do this on both machines, and only create one project per machine. I recommend you name the server machine's project: ChatServer and the client machine's project: ChatClient since I will be using those names for my projects.
4. Once you have both projects, right click on the right side on the solution explorer under the folders at the top(so in the free grey space) and then click "Add" -> then click "Existing Item..." -> then double click either your server.c or client.c whichever machine you are doing it on. The server.c goes in the ChatServer project and the client.c goes in the ChatClient project.
5. Now you want to build both your server.c and client.c, so on both machines make sure you're clicked into the code editor and hit all at the same time, ctrl+shift+B, you should see at the bottom of your screen it shows you a build success message and how long it took to build. Make sure to do this on both machines.
6. Now that they are built, you should be good to run them in the command prompt. Go to "Tools" in the top nav bar and click on "Command Line" -> then "Developer Command Prompt"
7. Now you should see the command prompt on both machines. You need to redirect to where your .exe is, so paste this into your server command prompt, make sure to keep the quotes around the path: 'cd /d "C:\Users\ponyt\source\repos\ChatServer\x64\Debug"' and then change the "ponyt" part to whatever your 5 letter user is in your file directory. Now do the same on your client machine but with 'cd /d "C:\Users\ponyt\source\repos\ChatClient\x64\Debug"'.
8. If you have any issues with the file paths, you can always go to file explorer click on "This PC" then your C Drive then click "Users" then find whichever folder in there has the "source" folder -> then "repos" folder -> then you should see your ChatServer/ChatClient project go into that folder -> "x64" folder -> "Debug" folder -> then you should see the .exe and then copy the file path to get to the Debug folder like my path above. Do not forget the cd /d part and the quotations around your file path.
9. Now, let's start with the server first, on the server machine command prompt, type: "ChatServer.exe 55" after your file path it will probably popup a firewall warning asking for private network connections, make sure to allow that. If you hit cancel or deny, you'll have to look up how to change that back to allow.
10. Now that the server is running, you should see this message:
"
Initializing Winsock.
Bind done
Waiting for incoming connection on port 55.
"
if you do not, then double check the steps again.
11. Now that the server is listening for traffic on port 55, we need to run the client, so on the client machine command prompt, in the Debug file path, type "ChatClient.exe [Input Your Server Machine's IPv4] 55" to find your server's IPv4 open a real command prompt and type in "ipconfig" and look for IPv4 and then use that. It should output:
"
Initializing Winsock.
Connecting to [Your IP]:55.
Connected successfully!
Chat started! You can start composing your first message.

Enter a message:
"
12. Now you can begin the chat! Each user gets a turn to talk and only relinquishes their turn when they type just "#" or "Exit". Once "#" is sent then all messages already lined up will send to the other user all at once and the other user will have the option to respond. Once "Exit" is sent, all messages beforehand are sent over and the other user is told that you want to exit, but the two users can continue to talk to each other until both users send the "Exit" command. You can even resend the "Exit" command to tell the other user again that you want to exit. (small note but # also allows the receiving messages user to start composing their responce back to the other user and this must be entered before anything can happen, even the "Exit" command)