(() => {
"use strict";
const $ = id => document.getElementById(id);
const carCanvas=$("carCanvas"), pianoCanvas=$("pianoCanvas"), speakerCanvas=$("speakerCanvas");
const cx=carCanvas.getContext("2d"), px=pianoCanvas.getContext("2d"), sx=speakerCanvas.getContext("2d");
let audioCtx=null, analyser=null, source=null, bins=null, live=false, demo=true, carOn=true;
const carReference = new Image(); carReference.src = "assets/batmobile-car-reference.png";
let mode="cruise", carX=100, direction=1, spin=0, time=0, low=0, mid=0, high=0, overall=0;
let intensity=1;
const clamp=(v,a=0,b=1)=>Math.max(a,Math.min(b,v));
function setMode(m){mode=m;document.querySelectorAll(".mode").forEach(b=>b.classList.toggle("active",b.dataset.mode===m));$("carModeLabel").textContent="MODE: "+m.toUpperCase().replace("CRUISE","BACK & FORTH");}
document.querySelectorAll(".mode").forEach(b=>b.addEventListener("click",()=>setMode(b.dataset.mode)));
$("demoBtn").onclick=()=>{demo=!demo;$("demoBtn").textContent="DEMO MODE: "+(demo?"ON":"OFF");$("demoBtn").classList.toggle("active",demo);};
$("carBtn").onclick=()=>{carOn=!carOn;$("carBtn").classList.toggle("active",carOn);};
$("intensity").oninput=e=>intensity=Number(e.target.value)/100;
$("audioBtn").onclick=async()=>{
 try{
  const stream=await navigator.mediaDevices.getUserMedia({audio:{echoCancellation:false,noiseSuppression:false,autoGainControl:false},video:false});
  audioCtx=new (window.AudioContext||window.webkitAudioContext)();
  analyser=audioCtx.createAnalyser();analyser.fftSize=4096;analyser.smoothingTimeConstant=.72;
  source=audioCtx.createMediaStreamSource(stream);source.connect(analyser);bins=new Uint8Array(analyser.frequencyBinCount);
  live=true;demo=false;$("audioLabel").textContent="LIVE INPUT";$("audioState").style.background="#36e28c";
  $("demoBtn").textContent="DEMO MODE: OFF";$("demoBtn").classList.remove("active");
 }catch(err){alert("Audio input could not be opened. Try HTTPS/localhost in a supported browser and select a loopback input such as Stereo Mix or a virtual cable. Details: "+err.message);}
};
function sampleAudio(){
 if(live&&analyser){analyser.getByteFrequencyData(bins);const hz=audioCtx.sampleRate/analyser.fftSize;
  const band=(lo,hi)=>{let a=Math.max(0,Math.floor(lo/hz)),b=Math.min(bins.length-1,Math.ceil(hi/hz)),s=0,n=0;for(let i=a;i<=b;i++){s+=bins[i];n++;}return n?s/n/255:0;};
  low=band(25,120);mid=band(120,4000);high=band(4000,15000);overall=band(25,15000);
 }else if(demo){const beat=Math.pow(Math.max(0,Math.sin(time*2.5)),8);low=.12+.72*beat;mid=.12+.25*(.5+.5*Math.sin(time*3.1));high=.08+.3*Math.pow(Math.max(0,Math.sin(time*13)),5);overall=(low+mid+high)/3;}
 else{low*=.92;mid*=.92;high*=.92;overall*=.92;}
}
function drawCar(){
 const w=carCanvas.width,h=carCanvas.height;cx.clearRect(0,0,w,h);
 const g=cx.createLinearGradient(0,0,0,h);g.addColorStop(0,"#09070a");g.addColorStop(1,"#16070b");cx.fillStyle=g;cx.fillRect(0,0,w,h);
 // Matrix rain and runway
 for(let i=0;i<90;i++){let x=(i*157+time*35)%w,y=(i*43+time*85)%h;cx.fillStyle=`rgba(255,25,50,${.08+(i%4)*.025})`;cx.font="12px monospace";cx.fillText(i%2?"1":"0",x,y);}
 cx.fillStyle="#210910";cx.fillRect(0,235,w,65);cx.strokeStyle="#8d1d2d";cx.lineWidth=2;cx.beginPath();cx.moveTo(0,238);cx.lineTo(w,238);cx.stroke();
 for(let i=0;i<18;i++){let x=(i*100+time*180)%w;cx.fillStyle="rgba(255,40,55,.24)";cx.fillRect(x,260,42,2);}
 if(carOn){
  if(mode==="cruise"){carX+=direction*(2.2+overall*2);if(carX> w-270)direction=-1;if(carX<60)direction=1;}
  if(mode==="burnout"){spin+=.24+low*.8; carX=Math.max(70,Math.min(w-520,carX));}
  if(mode==="donut"){spin+=.055+low*.08;carX=w/2+Math.cos(time*1.8)*100;}
  if(mode==="idle"){}
  if(mode==="cruise")spin+=.09;
  drawReferenceCar(cx,mode==="donut"?w/2-240:carX,mode==="donut"?85:85,mode==="donut"?spin:0);
  if(mode==="burnout"||mode==="donut"){
   for(let i=0;i<30;i++){let x=carX+(Math.random()-.5)*260,y=210-Math.random()*90;cx.fillStyle=`rgba(190,190,190,${Math.random()*.12})`;cx.beginPath();cx.arc(x,y,8+Math.random()*24,0,Math.PI*2);cx.fill();}
  }
 }
 cx.fillStyle="#e9b86a";cx.font="bold 14px Consolas";cx.fillText("CHICAGO BEATS • 28-INCH RIM RUNWAY",20,28);
}
function drawReferenceCar(ctx,x,y,rotation){
 if(!carReference.complete || !carReference.naturalWidth){drawVectorCar(ctx,x,y,rotation);return;}
 ctx.save();ctx.translate(x+220,y+65);ctx.rotate(rotation);
 const scale=Math.min(440/carReference.width,145/carReference.height);
 ctx.shadowColor="#ff1d35";ctx.shadowBlur=18;
 ctx.drawImage(carReference,-carReference.width*scale/2,-carReference.height*scale/2,carReference.width*scale,carReference.height*scale);
 ctx.restore();
}
function drawVectorCar(ctx,x,y,rotation){
 ctx.save();ctx.translate(x+110,y+45);ctx.rotate(rotation);
 // low futuristic silhouette; replace with extracted transparent reference sprite in production pass
 ctx.shadowColor="#ff1d35";ctx.shadowBlur=18;ctx.fillStyle="#090b10";ctx.strokeStyle="#9aa4b1";ctx.lineWidth=3;
 ctx.beginPath();ctx.moveTo(-115,30);ctx.lineTo(-94,-6);ctx.lineTo(-45,-28);ctx.lineTo(25,-31);ctx.lineTo(86,-12);ctx.lineTo(119,12);ctx.lineTo(132,30);ctx.closePath();ctx.fill();ctx.stroke();
 ctx.shadowBlur=0;ctx.fillStyle="#14222a";ctx.beginPath();ctx.moveTo(-44,-25);ctx.lineTo(-12,-48);ctx.lineTo(39,-43);ctx.lineTo(77,-14);ctx.closePath();ctx.fill();ctx.stroke();
 ctx.strokeStyle="#ff293f";ctx.lineWidth=5;ctx.beginPath();ctx.moveTo(-75,0);ctx.lineTo(-45,7);ctx.moveTo(84,5);ctx.lineTo(116,17);ctx.stroke();
 for(const wx of [-72,77]){ctx.save();ctx.translate(wx,30);ctx.rotate(spin);ctx.fillStyle="#050505";ctx.strokeStyle="#c5c7ca";ctx.lineWidth=5;ctx.beginPath();ctx.arc(0,0,25,0,Math.PI*2);ctx.fill();ctx.stroke();ctx.strokeStyle="#e8b966";ctx.lineWidth=2;for(let i=0;i<5;i++){ctx.rotate(Math.PI*2/5);ctx.beginPath();ctx.moveTo(0,0);ctx.lineTo(0,-21);ctx.stroke();}ctx.restore();}
 ctx.restore();
}
function drawPiano(){
 const w=pianoCanvas.width,h=pianoCanvas.height;px.clearRect(0,0,w,h);px.fillStyle="#08070a";px.fillRect(0,0,w,h);
 // falling matrix
 for(let i=0;i<80;i++){let x=(i*137)%w,y=(i*47+time*70)%h;px.fillStyle="rgba(245,20,45,.18)";px.font="12px monospace";px.fillText(i%3?"0":"1",x,y);}
 // piano roll grid
 px.strokeStyle="rgba(255,50,70,.16)";for(let x=0;x<w;x+=50){px.beginPath();px.moveTo(x,0);px.lineTo(x,h-55);px.stroke();}for(let y=0;y<h-55;y+=25){px.beginPath();px.moveTo(0,y);px.lineTo(w,y);px.stroke();}
 // falling notes derived from mid/low signal
 for(let i=0;i<35;i++){let x=(i*83+Math.sin(i)*30)%w,y=(i*59+time*(70+low*170))%(h-65);let nh=18+(i%4)*9;px.shadowColor="#ff253e";px.shadowBlur=10;px.fillStyle=i%3?"#b51c31":"#e6b45d";px.fillRect(x,y,12,nh);px.shadowBlur=0;}
 // keyboard
 const keyW=w/48;for(let i=0;i<48;i++){let black=[1,3,6,8,10].includes(i%12);px.fillStyle=black?"#130a0d":"#d8c9c3";px.fillRect(i*keyW,h-53,keyW-1,53);if((i%12===0)&&low>.25){px.fillStyle=`rgba(255,25,55,${low*.7})`;px.fillRect(i*keyW,h-53,keyW-1,53);}}
 px.fillStyle="#e5b35d";px.font="bold 12px Consolas";px.fillText("MIDI NOTE FALL • PLAYHEAD FOLLOW • KEY IMPACTS",14,20);
}
function drawSpeaker(){
 const w=speakerCanvas.width,h=speakerCanvas.height;
 sx.clearRect(0,0,w,h);
 const bg=sx.createLinearGradient(0,0,0,h);bg.addColorStop(0,"#16070b");bg.addColorStop(1,"#050507");
 sx.fillStyle=bg;sx.fillRect(0,0,w,h);
 // Matrix code backdrop, deliberately subdued behind the hardware.
 for(let i=0;i<52;i++){const x=(i*47)%w,y=(i*31+time*42)%h;sx.fillStyle="rgba(255,24,48,.14)";sx.font="9px monospace";sx.fillText(i%2?"1":"0",x,y);}
 // Mirrored side spectrum panels.
 function spectrumPanel(x){
   sx.fillStyle="#09080b";sx.strokeStyle="#8f1c2c";sx.lineWidth=2;sx.fillRect(x,82,66,220);sx.strokeRect(x,82,66,220);
   for(let i=0;i<9;i++){
     const v=clamp((.12+.45*Math.abs(Math.sin(i*.62+time*2.1))+.4*high)*intensity/1.5);
     const bh=18+v*135;
     sx.fillStyle=i%3===0?"#f2c06b":"#e3223b";sx.shadowColor="#ff203c";sx.shadowBlur=6;
     sx.fillRect(x+7+i*6.1,285-bh,4,bh);sx.shadowBlur=0;
   }
   sx.strokeStyle="rgba(255,65,83,.5)";sx.beginPath();sx.moveTo(x+5,180);sx.lineTo(x+61,180);sx.stroke();
 }
 spectrumPanel(8);spectrumPanel(w-74);
 // Speaker driver: layered cabinet ring, surround, cone, dust cap.
 function driver(x,y,r,energy,large=false){
   const excursion=energy*(large?10:5);
   sx.save();sx.translate(x,y);
   sx.shadowColor="#ff1736";sx.shadowBlur=large?20:11;
   sx.fillStyle="#09090d";sx.strokeStyle="#aeb2ba";sx.lineWidth=large?5:3;
   sx.beginPath();sx.arc(0,0,r+7,0,Math.PI*2);sx.fill();sx.stroke();
   sx.shadowBlur=0;
   sx.strokeStyle="#8d1525";sx.lineWidth=large?8:5;sx.beginPath();sx.arc(0,0,r-5,0,Math.PI*2);sx.stroke();
   sx.strokeStyle="#f02b43";sx.lineWidth=2;sx.beginPath();sx.arc(0,0,r-13,0,Math.PI*2);sx.stroke();
   sx.fillStyle="#17151a";sx.beginPath();sx.arc(0,0,r-17+excursion*.15,0,Math.PI*2);sx.fill();
   sx.strokeStyle="#777780";sx.lineWidth=2;sx.beginPath();sx.arc(0,0,r-20+excursion*.15,0,Math.PI*2);sx.stroke();
   sx.fillStyle="#29252c";sx.beginPath();sx.arc(0,0,r*.34+excursion*.2,0,Math.PI*2);sx.fill();
   sx.strokeStyle="#d7a45a";sx.lineWidth=1.5;sx.beginPath();sx.arc(0,0,r*.34+excursion*.2,0,Math.PI*2);sx.stroke();
   sx.fillStyle="#e9b95f";sx.font=`${large?18:10}px Georgia`;sx.textAlign="center";sx.fillText("♛",0,large?7:4);
   sx.restore();
 }
 // central oversized crowned subwoofer and surrounding speaker array.
 driver(w/2,145,77,low,true);
 driver(132,112,30,low);driver(132,204,25,mid);
 driver(w-132,112,30,low);driver(w-132,204,25,mid);
 driver(218,62,18,high);driver(w-218,62,18,high);
 driver(218,265,20,low);driver(w-218,265,20,low);
 // vertical tower-stack accents at both sides of the central enclosure.
 for(const x of [78,w-78]){
   sx.fillStyle="#0b090d";sx.strokeStyle="#81202e";sx.lineWidth=2;sx.fillRect(x-18,88,36,180);sx.strokeRect(x-18,88,36,180);
   for(let j=0;j<4;j++){sx.beginPath();sx.arc(x,108+j*43,12,0,Math.PI*2);sx.fillStyle="#17141a";sx.fill();sx.strokeStyle="#b52a3d";sx.stroke();sx.beginPath();sx.arc(x,108+j*43,5,0,Math.PI*2);sx.fillStyle="#d7a65d";sx.fill();}
 }
 // Bass pressure ripples pulse outward from the crowned subwoofer.
 for(let k=0;k<3;k++){sx.strokeStyle=`rgba(255,38,58,${.2+low*.22})`;sx.lineWidth=2;sx.beginPath();sx.ellipse(w/2,145,105+k*18+low*15,92+k*12+low*12,0,0,Math.PI*2);sx.stroke();}
 // Readouts and title.
 sx.fillStyle="#e9b866";sx.font="bold 11px Consolas";sx.textAlign="left";
 sx.fillText("LOW "+Math.round(low*100)+"%",84,25);sx.fillText("MID "+Math.round(mid*100)+"%",235,25);
 sx.fillText("HIGH "+Math.round(high*100)+"%",390,25);
 sx.textAlign="center";sx.fillStyle="#f4d8dc";sx.font="bold 12px Consolas";sx.fillText("CHICAGO BEATS • BLOODLINE BASS ARRAY",w/2,h-12);
}
function frame(){time+=1/60;sampleAudio();drawCar();drawPiano();drawSpeaker();requestAnimationFrame(frame);}
setMode("cruise");frame();
})();