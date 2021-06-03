# Deployment instructions

To access an AWS EC2 instance you will need to use SSH. Linux and macOS support this in the terminal. On Windows you can use WSL or PowerShell.

1. Locate the keypair that you downloaded from AWS when you started your instance.
2. Find the IP address of the instance you would like to connect to. This will be shown if you click on a instance from the list you have created. In our example the IP address is `3.210.116.142`
3. Open a terminal.
4. Copy the keypair to your *~/.ssh* folder using `cp keypair.pem ~/.ssh/keypair.pem` where *keypair.pem* is downloaded keypair.
5. Set your key to have the correct permissions by running `chmod 0600 ~/.ssh/keypair.pem`. **Note** that you would need to run this **only** once.
6. Use the command `ssh ubuntu@3.210.116.142  -i ~/.ssh/keypair.pem`. Replace the IP address with your IP adddress of the instance.

You now have a terminal connected to your AWS instance.

7. Clone the github repository using `git clone https://github.com/spe-uob/historyTrail.git`. In case of any changes, make sure you get the latest version of the system before deployment using `git pull`.
8. Access the directory from where you can run the program using `cd historyTrail/History_Trails`
9. Run the following command `nohup sh ./mvnw spring-boot:run &`. Nohup is short for “No Hangups”. It is a command that tells the Linux system not to stop a command once it has started. That means it will keep running until it is done, even if the user that started it logs out (i.e.:  it allows you to start a process and not worry about whether it will finish if you log out or get disconnected from your server).

You now have your system deployed.
