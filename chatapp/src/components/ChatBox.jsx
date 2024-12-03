import React, { useState } from "react";

const ChatBox = ({ chat }) => {
  const [messages, setMessages] = useState([]);
  const [newMessage, setNewMessage] = useState("");

  const handleSend = () => {
    if (newMessage.trim()) {
      setMessages([...messages, { type: "sent", text: newMessage }]);
      setNewMessage("");
      // Simulate a received message
      setTimeout(() => {
        setMessages((prev) => [
          ...prev,
          { type: "received", text: "Got your message!" },
        ]);
      }, 1000);
    }
  };const { spawn } = require('child_process');
  const express = require('express');
  const cors = require('cors');
  const { WebSocketServer } = require('ws');
  
  // Initialize Express server
  const app = express();
  app.use(cors()); // Enable CORS if frontend and backend are on different origins
  const PORT = 3000;
  
  // Spawn the C executable
  const cProcess = spawn('./Server');
  
  // Array to store client data
  const clients = [];
  
  // Start the Express server
  const server = app.listen(PORT, () => {
      console.log(`Server is running on http://localhost:${PORT}`);
  });
  
  // Setup WebSocket server
  const wss = new WebSocketServer({ server });
  
  // Function to broadcast the updated client list to all connected WebSocket clients
  const broadcastClients = () => {
      const clientListJSON = JSON.stringify(clients);
      wss.clients.forEach((ws) => {
          if (ws.readyState === ws.OPEN) {
              ws.send(clientListJSON); // Send updated client list to WebSocket clients
          }
      });
  };
  
  // Setup Express route to get the current client list
  app.get('/clients', (req, res) => {
      console.log('HTTP GET Request for /clients');
      res.json(clients); // Fallback for non-WebSocket clients
  });
  
  // Log any output from the C process
  cProcess.stdout.on('data', (data) => {
      // Convert data to a string and split it by newlines
      const output = data.toString().split('\n');
  
      // Iterate through each line of output
      output.forEach(line => {
          // Skip empty lines or non-valid lines
          if (line.trim() === '') return;
  
          // Split the line into parts (index, name, status)
          const parts = line.split(' ');
  
          // If the line contains the expected number of parts, parse it
          if (parts.length === 3) {
              const index = parseInt(parts[0].trim(), 10) - 1; // Convert to zero-based index
              const name = parts[1].trim();
              const status = parseInt(parts[2].trim(), 10);
  
              if (status === 0) {
                  // Handle removal based on zero-based index
                  if (index >= 0 && index < clients.length) {
                      console.log(`Removing client at index ${index}:`, clients[index]);
                      clients.splice(index, 1);
                  } else {
                      console.error(`Invalid index for removal: ${index}`);
                  }
              } else {
                  // Add or update the client in the array
                  const client = {
                      name: name,
                      status: status
                  };
  
                  // Add client to the array
                  clients.push(client);
              }
          }
      });
  
      // Broadcast updated client list to WebSocket clients
      broadcastClients();
  
      // Log the current client list
      console.log('Updated clients:', clients);
  });
  
  // Log any errors from the C process
  cProcess.stderr.on('data', (data) => {
      console.error(`C Error: ${data}`);
  });
  
  // Handle process exit
  cProcess.on('close', (code) => {
      console.log(`C Process exited with code ${code}`);
  });
  
  // WebSocket connection handler
  wss.on('connection', (ws) => {
      console.log('A WebSocket client connected.');
  
      // Send the initial client list to the new WebSocket client
      ws.send(JSON.stringify(clients));
  
      // Handle WebSocket disconnection
      ws.on('close', () => {
          console.log('A WebSocket client disconnected.');
      });
  });
  

  return (
    <div className="chat-box">
      <div className="chat-header">{chat.name}</div>
      <div className="chat-messages">
        {messages.map((msg, index) => (
          <div
            key={index}
            className={`chat-message ${msg.type === "sent" ? "sent" : "received"}`}
          >
            {msg.text}
          </div>
        ))}
      </div>
      <div className="chat-input">
        <input
          type="text"
          value={newMessage}
          onChange={(e) => setNewMessage(e.target.value)}
          placeholder="Type a message"
        />
        <button onClick={handleSend}>Send</button>
      </div>
    </div>
  );
};

export default ChatBox;
