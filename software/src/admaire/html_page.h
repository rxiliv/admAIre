#pragma once

// ============================================================================
// WEB APP - CONSOLE DISPOSITIVO
// ============================================================================
const char PAGINA_HTML[] PROGMEM = R"HTMLPAGE(<!DOCTYPE html>
<html lang="it">
<head>
<meta charset="UTF-8">
<meta name="viewport" content="width=device-width, initial-scale=1.0, viewport-fit=cover">
<meta name="theme-color" content="#0f1115">
<title>AdmAIre</title>
<style>
:root{--bg:#0f1115;--text:#f1efe9;--dim:#9aa1ab;--violet:#6c52ad;--teal:#3ddc97;--danger:#ff5d5d}
*{box-sizing:border-box}
html,body{margin:0}
body{background:var(--bg);color:var(--text);font-family:-apple-system,BlinkMacSystemFont,"Segoe UI",Roboto,Helvetica,Arial,sans-serif;
  -webkit-font-smoothing:antialiased;max-width:640px;margin:0 auto;
  padding:calc(16px + env(safe-area-inset-top)) 16px calc(40px + env(safe-area-inset-bottom))}
.mono{font-family:ui-monospace,SFMono-Regular,Menlo,Consolas,monospace;font-variant-numeric:tabular-nums}
.dim{color:var(--dim)}

header{display:flex;align-items:baseline;justify-content:space-between;gap:10px;margin-bottom:14px}
h1{font-size:22px;margin:0;letter-spacing:.2px}
header nav{font-size:13px;display:flex;gap:12px;align-items:center;white-space:nowrap}
#netTxt{color:var(--teal)}#netTxt::before{content:"● ";font-size:10px}
#netTxt.off{color:var(--danger)}

h2{font-size:12px;font-weight:700;letter-spacing:1.4px;text-transform:uppercase;color:var(--dim);margin:0 0 10px}
hr{border:none;border-top:1px solid #262a31;margin:20px 0}

button,input,select{font:inherit;color:inherit}
button{cursor:pointer;-webkit-tap-highlight-color:transparent}
button:focus-visible,input:focus-visible,select:focus-visible{outline:2px solid var(--violet);outline-offset:2px}

.scan{width:100%;padding:20px;border:none;border-radius:12px;font-size:17px;font-weight:800;letter-spacing:.4px;
  background:var(--violet);color:#1a1306;display:flex;align-items:center;justify-content:center;gap:10px}
.scan:active{transform:scale(.98)}
.scan:disabled{filter:grayscale(.6) brightness(.7);cursor:progress}
.status-line{margin-top:10px;font-size:15px;font-weight:600;min-height:20px}
.status-line.err{color:var(--danger)}
.busy .status-line::after{content:"";animation:dots 1.2s steps(4) infinite}
@keyframes dots{0%{content:""}25%{content:"."}50%{content:".."}75%{content:"..."}}
.auto{display:flex;align-items:center;justify-content:space-between;gap:12px;margin-top:14px;font-size:14px}
.auto input{width:18px;height:18px;accent-color:var(--teal)}
.auto select{background:transparent;border:none;font-size:14px;text-align:right}

.question{font-size:13px;color:var(--dim);margin-bottom:4px}
.answer{font-size:22px;line-height:1.4;font-weight:600;min-height:60px;margin-top:8px;word-wrap:break-word}
.answer.empty{color:var(--dim);font-weight:400;font-size:16px}
.answer.err{color:var(--danger)}
.meta{font-size:12px;color:var(--dim);margin-top:8px}
.photo img{display:block;width:100%;border-radius:10px;margin-top:10px}
.btn{padding:8px 14px;border-radius:10px;border:1px solid #2a2f38;background:transparent;font-size:13px;font-weight:600}
.btn:disabled{opacity:.4}
.row-top{display:flex;justify-content:space-between;align-items:baseline;gap:10px}

.ask{display:flex;gap:8px}
.ask input{flex:1;min-width:0;padding:13px 0;border:none;border-bottom:1px solid #2a2f38;background:transparent;font-size:16px}
.ask input:focus{border-bottom-color:var(--violet);outline-offset:-2px}
.ask button{background:transparent;border:none;color:var(--violet);font-weight:700;font-size:15px;padding:0 4px}
.quick{display:flex;flex-wrap:wrap;gap:8px;margin-top:12px}
.quick button{padding:7px 12px;border-radius:999px;border:1px solid #2a2f38;background:transparent;font-size:13px;color:var(--dim)}
.quick button:hover{color:var(--text);border-color:var(--dim)}

.row{display:flex;justify-content:space-between;gap:10px;font-size:14px;color:var(--dim);padding:7px 0;border-bottom:1px dashed #262a31}
.row:last-child{border-bottom:none}
.row b{color:var(--text);font-weight:600;text-align:right}
.bat{display:flex;align-items:baseline;gap:14px}
.bat .pct{font-size:42px;font-weight:800}
.bat .ico{font-size:15px;color:var(--dim)}
.low .pct{color:var(--danger)}
.note{font-size:12px;color:var(--dim);margin-top:10px;line-height:1.5}
.vol{display:flex;align-items:center;gap:12px;margin-top:12px;font-size:14px;color:var(--dim)}
.vol input{flex:1;accent-color:var(--violet)}

details{margin:20px 0}
summary{cursor:pointer;list-style:none;font-size:12px;font-weight:700;letter-spacing:1.4px;text-transform:uppercase;color:var(--dim)}
summary::-webkit-details-marker{display:none}
summary::after{content:" ▾"}details[open] summary::after{content:" ▴"}
.log{margin-top:10px;max-height:200px;overflow-y:auto;font-size:12.5px;line-height:1.6}
.log .t{color:var(--dim);margin-right:8px}.log .err{color:var(--danger)}.log .ai{color:var(--teal)}
.sr{position:absolute;width:1px;height:1px;overflow:hidden;clip:rect(0 0 0 0)}
</style>
</head>
<body>
<header>
  <h1>AdmAIre</h1>
  <nav>
    <span id="batPillTxt">--%</span>
    <span id="netTxt">Connessione…</span>
  </nav>
</header>

<main id="hero">
  <button class="scan" id="scanBtn" onclick="scansiona()">
    <svg width="22" height="22" viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2.4" stroke-linecap="round" aria-hidden="true"><circle cx="12" cy="12" r="3.5"/><path d="M3 7V5a2 2 0 0 1 2-2h2M17 3h2a2 2 0 0 1 2 2v2M21 17v2a2 2 0 0 1-2 2h-2M7 21H5a2 2 0 0 1-2-2v-2"/></svg>
    <span id="scanLbl">Scansiona</span>
  </button>
  <div class="status-line" id="statusLine" aria-live="polite">Pronto</div>
  <div class="auto">
    <label><input type="checkbox" id="autoChk" onchange="toggleAuto()"> Scansione automatica</label>
    <select id="autoIntSel" aria-label="Intervallo">
      <option value="20">ogni 20 s</option><option value="30">ogni 30 s</option><option value="60">ogni 60 s</option>
    </select>
  </div>
</main>

<hr>
<section aria-label="Fai una domanda">
  <h2>Fai una domanda</h2>
  <form class="ask" onsubmit="event.preventDefault();chiedi()">
    <label for="qIn" class="sr">Domanda</label>
    <input id="qIn" type="text" maxlength="300" placeholder="Es. che autobus sta arrivando?" autocomplete="off" enterkeyhint="send">
    <button id="askBtn" type="submit">Chiedi</button>
  </form>
  <div class="quick">
    <button onclick="chiedi('Descrivi cosa ho davanti')">Cosa ho davanti?</button>
    <button onclick="chiedi('Leggi tutto il testo visibile')">Leggi il testo</button>
    <button onclick="chiedi('Ci sono persone vicine? Dove?')">Persone vicine</button>
    <button onclick="chiedi('Di che colore è l\'oggetto al centro?')">Colore</button>
  </div>
</section>

<hr>
<section aria-label="Ultima risposta">
  <div class="row-top">
    <h2>Ultima risposta</h2>
    <button class="btn" id="repeatBtn" onclick="ripeti()" disabled>Ripeti audio</button>
  </div>
  <div class="question" id="qTxt" hidden></div>
  <div class="answer empty" id="answer" aria-live="assertive">Nessuna analisi ancora. Premi “Scansiona”.</div>
  <div class="meta mono" id="meta" hidden><span id="msChip"></span> · <span id="timeChip"></span> · <span id="modelChip"></span></div>
  <div class="photo" id="photo"><img id="photoImg" alt="Immagine analizzata dalla fotocamera" hidden></div>
</section>

<hr>
<section aria-label="Batteria">
  <h2>Batteria</h2>
  <div class="bat" id="batBox">
    <div class="pct mono" id="batPct">--</div>
    <div class="ico" id="batIco">—</div>
  </div>
  <div class="rows" style="margin-top:10px">
    <div class="row"><span>Stato</span><b id="batState">—</b></div>
    <div class="row"><span>Tensione</span><b class="mono" id="batMv">—</b></div>
    <div class="row"><span>Stima</span><b id="batTime">—</b></div>
  </div>
  <div class="note" id="batNote" hidden>Lo stato di carica è stimato dall'andamento della tensione: può servire qualche minuto per rilevare il cavo. Durante la carica la percentuale è indicativa.</div>
</section>

<hr>
<section aria-label="Dispositivo">
  <h2>Dispositivo</h2>
  <div class="row"><span>Wi-Fi</span><b class="mono" id="rssi">—</b></div>
  <div class="row"><span>Acceso da</span><b class="mono" id="uptime">—</b></div>
  <div class="row"><span>Memoria libera</span><b class="mono" id="heap">—</b></div>
  <div class="vol">
    <label for="vol">Volume</label>
    <input type="range" id="vol" min="0" max="21" step="1" value="21" onchange="impostaVolume(this.value)">
    <b class="mono" id="volTxt" style="width:24px;text-align:right;color:var(--text)">21</b>
  </div>
</section>

<details><summary>Registro eventi</summary><div class="log mono" id="log" role="log"></div></details>

<script>
const $=id=>document.getElementById(id);
let lastSeq=null,busy=false,waiting=false,online=false,autoOn=false,autoInt=20,nextAuto=0,pollTimer=null,volDirty=false;

function log(msg,cls){
  const d=document.createElement('div');
  d.innerHTML='<span class="t"></span><span></span>';
  d.firstChild.textContent=new Date().toLocaleTimeString('it-IT');
  d.lastChild.textContent=msg;if(cls)d.lastChild.className=cls;
  $('log').appendChild(d);$('log').scrollTop=1e9;
}
function setStatus(t,err){const s=$('statusLine');s.textContent=t;s.classList.toggle('err',!!err);}
function fmtUp(s){const h=Math.floor(s/3600),m=Math.floor(s%3600/60);return h?h+' h '+m+' min':m+' min '+(s%60)+' s';}
async function getJSON(url,ms){
  const c=new AbortController();const t=setTimeout(()=>c.abort(),ms||5000);
  try{const r=await fetch(url,{signal:c.signal,cache:'no-store'});return {status:r.status,data:await r.json().catch(()=>({}))};}
  finally{clearTimeout(t);}
}

function renderBattery(b){
  const pct=b.detected?b.pct:null;
  $('batPct').textContent=pct===null?'--':pct+'%';
  $('batPillTxt').textContent=(b.charging?'⚡ ':'')+(pct===null?'--':pct+'%');
  $('batIco').textContent=!b.detected?'assente':b.charging?'⚡ in carica':'a batteria';
  $('batState').textContent=!b.detected?'Non rilevata':b.charging?(b.pct>=99?'Carica completa':'In carica'):'A batteria';
  $('batMv').textContent=b.detected?(b.mv/1000).toFixed(2)+' V':'—';
  $('batTime').textContent=b.time||'—';
  $('batNote').hidden=!b.estimated;
  $('batBox').classList.toggle('low',!!(b.detected&&!b.charging&&pct<=20));
}
function renderDevice(s){
  $('rssi').textContent=s.rssi+' dBm';
  $('uptime').textContent=fmtUp(s.uptime);
  $('heap').textContent=Math.round(s.heap/1024)+' KB'+(s.psram?' · PSRAM '+Math.round(s.psram/1048576*10)/10+' MB':'');
  if(!volDirty){$('vol').value=s.volume;$('volTxt').textContent=s.volume;}
  $('modelChip').textContent=s.model;
}
function renderResult(s,isNew){
  if(!s.seq)return;
  const a=$('answer');
  const err=!!s.error;
  a.textContent=err?s.error:s.text;
  a.className='answer'+(err?' err':'');
  $('qTxt').hidden=!s.question;$('qTxt').textContent=s.question?'Domanda: '+s.question:'';
  $('meta').hidden=false;
  $('msChip').textContent='⏱ '+(s.ms/1000).toFixed(1)+' s';
  $('timeChip').textContent=new Date().toLocaleTimeString('it-IT',{hour:'2-digit',minute:'2-digit'});
  $('repeatBtn').disabled=err||!s.text;
  if(s.photo){const img=$('photoImg');img.onload=()=>{img.hidden=false;};img.src='/photo.jpg?'+s.seq;}
  if(isNew){
    log(err?'Errore: '+s.error:s.text,err?'err':'ai');
    if(navigator.vibrate)navigator.vibrate(err?[80,60,80]:60);
  }
}
function setBusy(b){
  busy=b;document.getElementById('hero').classList.toggle('busy',b);
  $('scanBtn').disabled=b;$('askBtn').disabled=b;
  $('scanLbl').textContent=b?'Analisi…':'Scansiona';
}

async function poll(){
  clearTimeout(pollTimer);
  try{
    const {data:s}=await getJSON('/status',4000);
    if(!online){online=true;$('netTxt').textContent='Online';$('netTxt').classList.remove('off');log('Dispositivo connesso.');}
    renderBattery(s.battery);renderDevice(s);
    const isNew=lastSeq!==null&&s.seq!==lastSeq;
    if(lastSeq===null||isNew)renderResult(s,isNew);
    lastSeq=s.seq;
    if(isNew)waiting=false;
    setBusy(s.busy||waiting);
    if(!busy){
      if(isNew&&s.error)setStatus(s.error,true);
      else if(s.speaking)setStatus('Riproduzione vocale…');
      else if(autoOn)setStatus('Prossima scansione tra '+Math.max(0,Math.ceil((nextAuto-Date.now())/1000))+' s');
      else setStatus('Pronto');
    }else setStatus('Analisi in corso');
  }catch(e){
    if(online)log('Connessione persa con il dispositivo.','err');
    online=false;$('netTxt').textContent='Offline';$('netTxt').classList.add('off');setStatus('Dispositivo non raggiungibile',true);
  }
  if(autoOn&&online&&!busy&&Date.now()>=nextAuto){avvia('',true);return;}
  clearTimeout(pollTimer);
  pollTimer=setTimeout(poll,busy?400:(autoOn?1000:2000));
}

async function avvia(q,auto){
  if(busy)return;
  nextAuto=Date.now()+autoInt*1000;
  waiting=true;setBusy(true);setStatus(q?'Elaboro la domanda':'Analisi in corso');
  if(!auto)log(q?'Domanda: '+q:'Scansione avviata.');
  try{
    const r=await getJSON('/analyze?'+(auto?'auto=1&':'')+'q='+encodeURIComponent(q||''),5000);
    if(r.status===409)log('Un\'analisi è già in corso.');
  }catch(e){waiting=false;setBusy(false);setStatus('Impossibile avviare l\'analisi',true);log('Errore di rete.','err');}
  clearTimeout(pollTimer);
  poll();
}
function scansiona(){avvia('',false);}
function chiedi(q){q=(q!==undefined?q:$('qIn').value).trim();if(!q)return $('qIn').focus();$('qIn').value='';avvia(q,false);}
async function ripeti(){try{await fetch('/repeat');log('Ripetizione vocale.');}catch(e){}}
async function impostaVolume(v){volDirty=true;$('volTxt').textContent=v;try{await fetch('/volume?v='+v);}catch(e){}volDirty=false;}
$('vol').oninput=e=>{volDirty=true;$('volTxt').textContent=e.target.value;};

function toggleAuto(){
  autoOn=$('autoChk').checked;nextAuto=Date.now()+1500;
  log(autoOn?'Scansione automatica attiva ogni '+autoInt+' s.':'Scansione automatica disattivata.');
  poll();
}
$('autoIntSel').onchange=e=>{autoInt=+e.target.value;nextAuto=Math.min(nextAuto,Date.now()+autoInt*1000);};
document.addEventListener('visibilitychange',()=>{if(!document.hidden)poll();});

log('Console avviata.');
poll();
</script>
</body>
</html>
)HTMLPAGE";
