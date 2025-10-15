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
    res.sendFile(__dirname+"/public/index.html")  
})

let wss= new WebSocket.Server({server});

wss.on("connection",ws=>{
    console.log("client connected to server");

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