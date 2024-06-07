<head>
    <h1>
        <p align="center">
            <a href="https://ucode.world/en/" target="_blank">
                <img src="https://github.com/Strawberry-Team/ucode-connect-Track-C-uchat/blob/develop/screens/ucode_it_academy_logo.svg" height="100px" alt="ucode_logo">
            </a>
        </p>
    </h1>
    <h3>
        <p align="center"><strong>ucode connect uchat "Speechy"</strong></p>
        <p align="center"><sub><i>ucode connect uchat is the final stage of ucode connect the Track C programming bootcamp<br />
            lasting 11 weeks (08.02.2024 - 24.04.2024),<br />where the C programming language, 
            SQLite3 and GTK+3 libraries were used to develop the real-time communication tool.<br /><br />
            The purpose: create a real-time communication tool using creativity<br />and the whole cycle of Challenge Based Learning framework with a team.</i></sub></p>
    </h3>
</head>



# "Strawberry" Team
<div>
    <ul>
        <li>
            <p>👩🏼‍💻 <a href="https://github.com/InessaRepeshko">Inessa Repeshko</a></p>
        </li>
        <li>
            <p>🧑🏻‍💻 <a href="https://github.com/Kolesnichenko0">Denys Kolesnychenko</a></p>
        </li>
        <li>
            <p>🧑🏼‍💻 <a href="https://github.com/laskevych">Andrew Laskevych</a></p>
        </li>
        <li>
            <p>🧑🏻‍💻 <a href="https://github.com/Forzenook">Anton Lukash</a></p>
        </li>
        <li>
            <p>🧑🏻‍💻 <a href="https://github.com/vzharyi">Vadym Zharyi</a></p>
        </li>
    </ul>
</div>
<div align="center">
    <img src="https://github.com/Strawberry-Team/ucode-connect-Track-C-uchat/blob/develop/screens/Strawberry-team.jpg" alt="Strawberry-team">
</div><br />



# About "Speechy" messaging app
<div align="left">
    <p><strong>"Speechy"</strong> is a desktop messaging application designed for seamless communication between users on a 
        localhost network, inspired by Telegram.This robust application consists of both a client and a server, 
        ensuring efficient handling of client requests and secure data storage in a database.
    </p>
    <p>The client side is crafted using GTK+ 3, Glade, and CSS 3, providing a visually appealing and 
        user-friendly interface. On the server side, the application is built with the C programming language, 
        incorporating OpenSSL for enhanced security and JSON for creating a streamlined API. SQLite3 is utilized 
        for the database, ensuring reliable and efficient data management.
    </p>
    <p> The application guarantees a secure and private messaging experience, making it an ideal solution for local 
        network communications.
    </p>
</div>
<div align="center">
    <img src="https://github.com/Strawberry-Team/ucode-connect-Track-C-uchat/blob/develop/screens/Speechy.png" alt="Speachy">
</div><br />



# Idea of the application
<div>
    <p>Our desktop messenger "Speechy" embodies the values of our team, which include minimalism, security, and stability.</p>
    <p>Our product strives for minimalism, providing users with a simple and elegant interface that promotes comfort and ease of use.</p>
    <p>One of our priorities is security, as we understand that the security of user data is a fundamental 
        aspect of trust. In addition, we prioritize the stability of the application to ensure a continuous and 
        uninterrupted user experience. In addition, we care about elegance and stylish design, intuitiveness and 
        functionality that promote ease of use, as well as night and day modes to adapt to the needs of users at 
        any time of the day.</p>
</div>
<div align="center">
    <img src="https://github.com/Strawberry-Team/ucode-connect-Track-C-uchat/blob/develop/screens/idea.jpg" alt="idea">
</div><br />



# Development tools
<div>
    <p>For the backend, the use of the C programming language together with OpenSSL libraries for data security, 
        Sqlite3 for storing and managing local data, and cJSON for processing and transferring data in JSON format 
        allowed us to create a powerful and reliable product with effective cryptographic protection, 
        the ability to save user data, and convenient data exchange between the client and server.</p>
    <p>GTK3+, Glade, and CSS3 were used to create the application's front-end, which ensures efficient creation and 
        customization of the user interface using a variety of graphic elements and styles that provide high 
        user-friendliness and aesthetic appearance.</p>
    <p>The use of development tools such as GitHub for code version management and collaboration, CLion for software 
        creation and debugging, Google Mets for convenient communication and remote project collaboration, 
        and Fibery for project management and team communication contributed to the effective development of our product, 
        ensuring convenient collaboration, process stability, and development speed.</p>
</div>
<div align="center">
    <img src="https://github.com/Strawberry-Team/ucode-connect-Track-C-uchat/blob/develop/screens/dev-tools.jpg" alt="dev-tools">
</div><br />



# Fibery
<div>
    <p>Using Fibery proved to be extremely beneficial for our product, as it allowed us to effectively manage the 
        development process and team interaction. Due to the Tasks board, we were able to systematize and prioritize 
        tasks, which contributed to increased productivity and timely completion of tasks. The GitHub commit board 
        helped us track the code development process. And Google Meet has become an effective tool for planning and 
        conducting meetings.Finally, the documentation board helped us keep information unified and accessible to all 
        team members. All of these Fibery features allowed us to effectively manage the development process and ensure 
        that the team worked on our product successfully and smoothly.</p>
</div>
<div align="center">
    <img src="https://github.com/Strawberry-Team/ucode-connect-Track-C-uchat/blob/develop/screens/Fibery.gif" alt="Fibery">
</div><br />



# Deployment Diagram
<div align="center">
    <img src="https://github.com/Strawberry-Team/ucode-connect-Track-C-uchat/blob/develop/screens/deployment-diagram.jpg" alt="deployment-diagram">
</div>
<div>
    <p>Our project uses a client-server architecture. On the diagram we can see a deployment diagram that helps us understand 
        how our application is physically distributed.</p>
    <p>The server and client communicate using TCP for a reliable connection at the transport layer, TLS/SSL for data 
        encryption and connection security, and JSON as the data exchange format.</p>
    <p>Our application works as follows: The client sends a request to the server in JSON format over a secure TLS/SSL 
        connection. The server receives the request from the client by reading the JSON string from the client's socket. 
        The server then analyzes the type of request, processes the request, and sends a response to the client 
        by generating a JSON string and sending it to the client over a TLS/SSL connection. When the client receives 
        the response from the server, it reads the JSON response string and processes it according to the request type. 
        This process is repeated for every request the client sends to the server.</p>
    <p>Our server also uses multithreading. A new thread is created for each client that connects to the server. 
        The main thread continues to listen for new connections, so it is not blocked by processing already connected 
        clients. This allows you to process several clients simultaneously. Our program also uses mutexes to synchronize 
        access to the log file, which records information messages and records of various types of errors from different 
        clients.</p>
    <p>Our server uses blocking sockets, in which the execution of a thread using a socket is 
        suspended until the socket operation is completed (e.g., sending or receiving data). We chose this type of 
        sockets because they do not require additional system resources, as would be the case with non-blocking sockets, 
        i.e. asynchronous programming. In addition, our chat application does not require a large amount of data or 
        a large number of simultaneous connections, so blocking sockets are the best choice for our application.</p>
    <p>Our server uses one socket to listen for new connections, and each client has its own socket to communicate with 
        the server.</p>
    <p>About the security of our application, namely password hashing. Our project uses the SHA-256 hashing algorithm to 
        create password hashes. This algorithm was chosen for several reasons. First, it provides a high level of 
        security, i.e., resistance to various attacks. Second, SHA-256 is widely used in many projects, such as Bitcoin, 
        to ensure blockchain security. Third, SHA-256 is supported by most cryptographic libraries, including OpenSSL, 
        which, as mentioned earlier, we use in our project. There are other hashing algorithms we could have chosen, 
        including MD5, SHA1, SHA3, and others. However, there are reasons why we did not choose them, if anyone is 
        interested, I will be happy to answer this question later.</p>
</div><br />



# Database Schema
<div align="center">
    <img src="https://github.com/Strawberry-Team/ucode-connect-Track-C-uchat/blob/develop/screens/database-schema.jpg" alt="database-schema">
</div>
<div>
    <p>We implemented a relational database using SQLite. During the database normalization process, 
        we achieved 3 normal forms. We added unique indexes and comparable unique indexes to delegate the deduplication 
        process to the database and speed up data retrieval in these tables.</p>
</div><br />



# UI/UX
<div>
    <p>To develop the client, we used tools such as: Glade and Gtk3+, which are two powerful tools that were used 
        to create an easy-to-understand and functional graphical user interface (GUI) for our chat. 
        We also used CSS3 to add style to our chat.</p>
    <p>Glade offers a user-friendly visual editor that allows you to drag and drop interface elements such as buttons, 
        input fields, and text boxes onto windows. This greatly simplifies the process of creating a graphical interface, 
        saving time and effort.</p>
    <p>It was also important for us that our program was not only functional, but also easy to use for our users.</p>
    <p>All elements were designed in such a way that a new user could easily navigate and use the application without additional explanations.</p>
    <p>When developing our chat, we actively applied the best UI/UX practices. It is important for us that our program 
        is not only functional but also easy to use for our users.</p>
    <p>We paid attention to the simplicity and intuitiveness of the interface. All elements were designed so that a new 
        user could easily navigate and use the application without any additional explanations.</p>
    <p>While developing the client application with GTK+3, we applied several UI and UX best practices to improve 
        the user experience and usability:</p>
    <ul>
        <li>Consistency: Ensure that design elements such as colors, fonts, and layouts are consistent across screens 
            and application components.</li>
        <li>Clarity and simplicity: Make the user interface simple and intuitive, avoiding clutter and unnecessary 
            elements to minimize cognitive load and make it easier for users to navigate.</li>
    </ul>
    <p>Another key component of our project was the use of CSS3 to add style to our chat. This technology gave us the 
        opportunity to get creative with the design of the chat interface, making it more attractive and aesthetically 
        pleasing. Thanks to CSS3, we were able to customize colors, fonts, and much more, creating a comfortable and 
        pleasant environment for communication.</p>
</div><br />



# Features

## Authorization, registration
<div>
    <p>From the beginning of the development, we created user-friendly registration and login windows so that our 
    users can quickly and easily access the chat. These windows also display any errors that may occur when
    entering incorrect data, which ensures their convenient use.</p>
</div>
<div align="center">
    <img src="https://github.com/Strawberry-Team/ucode-connect-Track-C-uchat/blob/develop/screens/login.gif" alt="login">
    <img src="https://github.com/Strawberry-Team/ucode-connect-Track-C-uchat/blob/develop/screens/registration.gif" alt="registration">
</div><br />

## Chats with users
<div>
    <p>In addition, we have successfully implemented the functionality of adding a new chat in a separate window. 
    This allows users to quickly find other registered users and create new chats with them.</p>
</div>
<div align="center">
    <img src="https://github.com/Strawberry-Team/ucode-connect-Track-C-uchat/blob/develop/screens/add_chat.gif" alt="add_chat">
</div><br />

## Change password
<div>
    <p>The ability to easily change your password has also been added. For added security, you need to enter 
    the new password twice to avoid mistakes. Notifications are also displayed when you change your password 
    or if there are any errors during the process.</p>
</div>

## Appearance themes
<div>
    <p>Another key feature of our chat is the ability to change the topic. We have provided a choice between 
        night and day themes for communication. This allows users to customize the appearance of the chat 
        according to their own preferences and needs. Whether you are working at night or during the day, 
        you can enjoy a comfortable chat in an environment that suits you.</p>
</div>
<div align="center">
    <img src="https://github.com/Strawberry-Team/ucode-connect-Track-C-uchat/blob/develop/screens/profile.gif" alt="profile">
</div><br />

## Messaging
<div>
    <p>Users see that messages are sent almost instantly, without delay, and messages can be written in any language.</p>
    <p>In our chat, users can see messages in the chat even if the connection to the server is lost.</p>
    <p>We have also added the ability to use a wide range of emoticons to express your emotions and make 
        messages more interesting. Our chat supports all emoticons in Unicode encoding.</p>
</div>

## Editing messages
<div>
    <p>Our chat has an option to edit messages. This gives you the opportunity to correct mistakes or add 
    information. Other users can see changes in messages by displaying the edit status next to the edited message. 
    This helps them to be aware of any changes to the message.</p>
</div>

## Deleting messages
<div>
    <p>The function of deleting messages in our chat is implemented as a soft delete, which means that the 
    message is removed from the user interface but saved in the database.</p>
</div>
<div align="center">
    <a href="https://www.youtube.com/watch?v=WwZyQJQH8EE" target="chats">
        <img src="https://img.youtube.com/vi/WwZyQJQH8EE/0.jpg" height="700px" alt="chats">
    </a>
</div>



# "Speechy" Demo
<div align="center">
    <a href="https://www.youtube.com/watch?v=FPBP0BB3xE0" target="demo">
        <img src="https://img.youtube.com/vi/FPBP0BB3xE0/0.jpg" height="700px" alt="demo">
    </a>
</div>



# Presentation
Here is a [link](https://docs.google.com/presentation/d/1cx73MW6W2gJM0PKssESfrSjnQ0xaxIa3/edit?usp=sharing) to the presentation file.</p>



# How to run
```shell
# Clone repository
git clone <repo_link>
 
# Run Makefile
make

# Run application
./uchat_server 8090
./uchat 127.0.0.1 8090
```



© Inessa Repeshko. 2024
