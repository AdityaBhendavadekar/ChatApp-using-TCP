# ChatApp using TCP

A robust and interactive chat application built using TCP socket programming for backend communication and a modern tech stack for the user interface.

## Overview
ChatApp is a real-time messaging platform that enables seamless communication between clients over a TCP-based network. It leverages the power of C for socket programming, Node.js for backend routing, and React.js for a sleek, user-friendly interface.

---

## Features
- **Real-Time Communication**: Enables dynamic and instant messaging between clients.
- **User Management**: Dynamically updates the list of connected users.
- **Interactive UI**: Intuitive and responsive design for better user experience.
- **Reliable Communication**: TCP ensures message delivery and connection integrity.

---

## Technologies Used

### **Backend:**
1. **C**  
   - Socket programming to handle TCP connections and facilitate real-time message exchange.
2. **Node.js**  
   - Acts as the middleware for routing, managing connections, and facilitating communication between the C-based server and the React frontend.

### **Frontend:**
1. **React.js**  
   - Builds the user interface, ensuring a responsive and interactive experience.
2. **CSS** & **Bootstrap**  
   - Styles and layouts for a modern, visually appealing design.

---

## Architecture
The application follows a client-server architecture:
1. **Server (C-based):** Handles TCP socket communication, manages client connections, and ensures reliable message delivery.
2. **Node.js Middleware:** Bridges the server and frontend, routing messages and client data.
3. **Frontend (React.js):** Displays real-time messages, connected users, and other interface components dynamically.

---

## Installation & Setup

### Prerequisites:
- GCC or any C compiler
- Node.js and npm
- A modern web browser

### Steps:
1. **Clone the repository:**
   ```bash
   git clone https://github.com/yourusername/ChatApp-using-TCP.git
   cd ChatApp-using-TCP
