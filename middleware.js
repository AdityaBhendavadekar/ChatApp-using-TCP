const express = require('express');
const bodyParser = require('body-parser');
const { spawn } = require('child_process');

const app = express();
app.use(bodyParser.json());

let connectedClients = []; // Stores the current list of clients

// Endpoint to accept the name and launch the client
app.post('/start-client', (req, res) => {
    const { name } = req.body;

    if (!name) {
        return res.status(400).send({ error: "Name is required." });
    }

    // Spawn the Client process
    const clientProcess = spawn('./Client');

    // Send the name to the C client
    clientProcess.stdin.write(name + '\n');

    clientProcess.stdout.on('data', (data) => {
        console.log(`Client Output: ${data.toString()}`);
    });

    clientProcess.stderr.on('data', (data) => {
        console.error(`Client Error: ${data.toString()}`);
    });

    clientProcess.on('close', (code) => {
        console.log(`Client Process exited with code ${code}`);
    });

    res.send({ message: `Client started with name: ${name}` });
});

// Endpoint to fetch the list of connected clients
app.get('/clients', (req, res) => {
    res.send({ clients: connectedClients });
});

// Start the server
app.listen(3001, () => {
    console.log("Middleware server running on port 3001");
});

// Hook into the Server process to update client list
const serverProcess = spawn('./Server');
serverProcess.stdout.on('data', (data) => {
    const output = data.toString().split('\n');

    output.forEach(line => {
        if (line.trim() === '') return;

        const parts = line.split(' ');

        if (parts.length === 3) {
            const index = parseInt(parts[0].trim(), 10) - 1;
            const name = parts[1].trim();
            const status = parseInt(parts[2].trim(), 10);

            if (status === 0) {
                if (index >= 0 && index < connectedClients.length) {
                    connectedClients.splice(index, 1);
                }
            } else {
                const client = { name, status };
                connectedClients.push(client);
            }
        }
    });
    console.log('Updated Clients:', connectedClients);
});

serverProcess.stderr.on('data', (data) => {
    console.error(`Server Error: ${data}`);
});
