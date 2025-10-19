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

wss.on("connection",(ws, req)=>{
    const ip = req.headers['x-forwarded-for']?.split(',')[0].trim() || req.socket.remoteAddress;
    const ua = req.headers['user-agent'];
    console.log(`WS client: ${ip} | ${ua}`);
    console.log("client connected to server");

    // Send the client's IP address to them
    ws.send(JSON.stringify({ type: 'client-info', ip: ip }));

    ws.on("message",(message)=>{
        
        broadcast(message)
    })
    
    ws.on("close",()=>{})
})

function broadcast(message){
    wss.clients.forEach((client)=>{
        if (client.readyState===client.OPEN) {  
            client.send(message.toString())
        }
    })
}