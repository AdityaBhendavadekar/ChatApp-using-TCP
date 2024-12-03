const { spawn } = require('child_process');
const readline = require('readline');

const express = require('express')
const router = express.Router();


// Spawn the Client.c executable
const clientProcess = spawn('./Client');

// Interface for user input and output
const rl = readline.createInterface({
    input: process.stdin,
    output: process.stdout,
});

// Handle data from the C client process
clientProcess.stdout.on('data', (data) => {
    process.stdout.write(data.toString());
});

// Handle errors from the C client process
clientProcess.stderr.on('data', (data) => {
    console.error(`C Client Error: ${data}`);
});

// Handle when the C client process exits
clientProcess.on('close', (code) => {
    console.log(`C Client process exited with code ${code}`);
    rl.close();
    process.exit(0);
});

// Read user input and send it to the C client process
rl.on('line', (input) => {
    clientProcess.stdin.write(input + '\n');
});

//for sending name
router.post('/register', async (req,res)=>{
    const {name} = req.body;

    if(name!=null) res.json(`name: ${name}`)
    // rl.on('line', (name) => {
    //     clientProcess.stdin.write(name + '\n');
    // });
})

module.exports = router