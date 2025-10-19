let express=require("express");
let WebSocket= require("ws");
let app = express()

app.use(express.static("public"))

let PORT=3000
let server =app.listen(PORT,()=>{
    console.log("hello there from "+ PORT);
})

app.get("/",(req,res)=>{
    console.log("hi im get");  
})

let wss= new WebSocket.Server({server});
let mcuIp = null; // Store NodeMCU IP address

wss.on("connection",(ws, req)=>{
    const ip = req.headers['x-forwarded-for']?.split(',')[0].trim() || req.socket.remoteAddress;
    const ua = req.headers['user-agent'];
    console.log(`WS client: ${ip} | ${ua}`);
    console.log("client connected to server");

    // Store client info on the websocket object
    ws.clientIp = ip;
    ws.userAgent = ua;

    // Check if this is the NodeMCU (arduino-WebSocket-Client or ESP8266 in user agent)
    if (ua && (ua.includes('ESP8266') || ua.includes('arduino') || ua.includes('WebSocket-Client'))) {
        mcuIp = ip;
        console.log(`NodeMCU detected at IP: ${mcuIp}`);
    }

    // Send the client's IP address to them
    ws.send(JSON.stringify({ type: 'client-info', ip: ip }));

    ws.on("message",(message)=>{
        const messageStr = message.toString();
        
        // Check if it's a log message (JSON with type: 'log')
        try {
            const data = JSON.parse(messageStr);
            if (data.type === 'log') {
                // Broadcast log to all clients except NodeMCU
                broadcastLog(data, ws);
            } else {
                // Regular command message (ON/OFF/TOGGLE/brightness)
                broadcast(messageStr);
            }
        } catch (e) {
            // Not JSON, treat as regular command
            broadcast(messageStr);
        }
    })
    
    ws.on("close",()=>{
        // If the disconnected client was the MCU, clear the stored IP
        if (ws.clientIp === mcuIp) {
            console.log("NodeMCU disconnected");
            mcuIp = null;
        }
    })
})

function broadcast(message){
    wss.clients.forEach((client)=>{
        if (client.readyState===WebSocket.OPEN) {  
            client.send(message.toString())
        }
    })
}

function broadcastLog(logData, senderWs){
    wss.clients.forEach((client)=>{
        if (client.readyState===WebSocket.OPEN) {
            // Don't send logs to NodeMCU (check by IP) and don't send back to sender
            if (client.clientIp !== mcuIp && client !== senderWs) {
                client.send(JSON.stringify(logData));
            }
        }
    })
}