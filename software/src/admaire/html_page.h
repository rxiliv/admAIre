#pragma once

// ============================================================================
// WEB APP - CONSOLE DISPOSITIVO
// ============================================================================
const char PAGINA_HTML[] PROGMEM = R"HTMLPAGE(<!DOCTYPE html>
<html lang="it">
<head>
<meta charset="UTF-8">
<meta name="viewport" content="width=device-width, initial-scale=1.0, viewport-fit=cover">
<title>AdmAIre — Console</title>
<style>
:root{
  --bg:#14161a;
  --surface:#1d2025;
  --surface-raised:#23272d;
  --line:#33373e;
  --text:#edeae2;
  --text-dim:#a4aab2;
  --amber:#f2a33d;
  --amber-dim:#8a6326;
  --teal:#4fd1ae;
  --danger:#e5484d;
  --radius:14px;
}
*{box-sizing:border-box;}
html,body{margin:0;padding:0;}
body{
  background:var(--bg);
  color:var(--text);
  font-family:-apple-system,BlinkMacSystemFont,"Segoe UI",Roboto,Helvetica,Arial,sans-serif;
  -webkit-font-smoothing:antialiased;
  min-height:100vh;
  padding:20px 16px 40px;
}
.mono{font-family:ui-monospace,SFMono-Regular,Menlo,Consolas,"Courier New",monospace;}
.wrap{max-width:420px;margin:0 auto;}

header.top{display:flex;align-items:center;justify-content:space-between;margin-bottom:22px;}
header.top .brand{display:flex;flex-direction:column;}
header.top .brand .name{font-size:22px;font-weight:800;letter-spacing:0.5px;}
header.top .brand .tag{font-size:11px;letter-spacing:1.5px;text-transform:uppercase;color:var(--text-dim);margin-top:2px;}
.status-dot{display:flex;align-items:center;gap:6px;font-size:11px;letter-spacing:1px;text-transform:uppercase;color:var(--teal);}
.status-dot .dot{width:8px;height:8px;border-radius:50%;background:var(--teal);box-shadow:0 0 0 3px rgba(79,209,174,0.15);}

.eyebrow{font-size:11px;font-weight:700;letter-spacing:1.5px;text-transform:uppercase;color:var(--text-dim);margin:0 0 10px 2px;}
section{margin-bottom:22px;}
.panel{background:var(--surface);border:1px solid var(--line);border-radius:var(--radius);padding:20px;}

.telemetry{display:flex;align-items:center;gap:20px;}
.gauge{position:relative;width:96px;height:96px;flex-shrink:0;}
.gauge svg{transform:rotate(-90deg);}
.gauge-track{fill:none;stroke:var(--line);stroke-width:8;}
.gauge-value{fill:none;stroke:var(--teal);stroke-width:8;stroke-linecap:round;transition:stroke-dashoffset 0.6s ease, stroke 0.3s ease;}
.gauge-label{position:absolute;inset:0;display:flex;align-items:center;justify-content:center;flex-direction:column;}
.gauge-label .pct{font-size:20px;font-weight:800;}
.gauge-label .icon{font-size:13px;margin-top:2px;}
.tele-info{flex:1;min-width:0;}
.tele-info .row{display:flex;justify-content:space-between;font-size:12px;color:var(--text-dim);padding:4px 0;border-bottom:1px dashed var(--line);}
.tele-info .row:last-child{border-bottom:none;}
.tele-info .row span:last-child{color:var(--text);}

.scan-btn{
  position:relative;width:100%;display:flex;align-items:center;justify-content:center;gap:12px;
  background:var(--surface-raised);border:1px solid var(--amber-dim);color:var(--amber);
  font-size:15px;font-weight:700;letter-spacing:0.5px;text-transform:uppercase;
  padding:18px;border-radius:12px;cursor:pointer;overflow:hidden;
  -webkit-tap-highlight-color:transparent;
}
.scan-btn:active{transform:scale(0.98);}
.scan-btn:disabled{color:var(--text-dim);border-color:var(--line);cursor:not-allowed;}
.scan-btn .ring{width:20px;height:20px;border-radius:50%;border:2px solid currentColor;position:relative;flex-shrink:0;}
.scan-btn.scanning .ring{
  background:conic-gradient(from 0deg, var(--amber) 0deg, transparent 90deg, transparent 360deg);
  animation:sweep 1s linear infinite;border-color:transparent;
}
.scan-btn.scanning .ring::after{content:"";position:absolute;inset:3px;border-radius:50%;background:var(--surface-raised);}
@keyframes sweep{to{transform:rotate(360deg);}}

.preview-frame{margin-top:16px;border-radius:10px;overflow:hidden;border:1px solid var(--line);background:#000;display:none;}
.preview-frame img{display:block;width:100%;}

.log{background:#0b0c0e;border:1px solid var(--line);border-radius:12px;padding:14px 16px;height:150px;overflow-y:auto;font-size:12.5px;line-height:1.6;}
.log .entry{border-bottom:1px solid #1a1c20;padding-bottom:8px;margin-bottom:8px;}
.log .entry:last-child{border-bottom:none;margin-bottom:0;padding-bottom:0;}
.log .t{color:var(--text-dim);margin-right:8px;}
.log .msg{color:var(--text);}
.log .msg.err{color:var(--danger);}
.log .msg.ai{color:var(--teal);}

button:focus-visible, .scan-btn:focus-visible{outline:2px solid var(--amber);outline-offset:3px;}
@media (prefers-reduced-motion: reduce){.scan-btn.scanning .ring{animation:none;}}
</style>
</head>
<body>
<div class="wrap">
  <header class="top">
    <div class="brand">
      <div class="name">AdmAIre</div>
      <div class="tag">Console dispositivo</div>
    </div>
    <div class="status-dot"><span class="dot"></span>Online</div>
  </header>

  <section>
    <div class="eyebrow">Telemetria</div>
    <div class="panel telemetry">
      <div class="gauge">
        <svg viewBox="0 0 96 96" width="96" height="96">
          <circle class="gauge-track" cx="48" cy="48" r="42"></circle>
          <circle class="gauge-value" id="gaugeArc" cx="48" cy="48" r="42" stroke-dasharray="264" stroke-dashoffset="264"></circle>
        </svg>
        <div class="gauge-label">
          <div class="pct mono" id="batVal">--%</div>
          <div class="icon" id="chgIcon">●</div>
        </div>
      </div>
      <div class="tele-info">
        <div class="row"><span>Stato</span><span id="batState">—</span></div>
        <div class="row"><span>Autonomia</span><span id="batTime" class="mono">—</span></div>
        <div class="row"><span>Rete</span><span class="mono">connesso</span></div>
      </div>
    </div>
  </section>

  <section>
    <div class="eyebrow">Comando</div>
    <div class="panel">
      <button id="snapBtn" class="scan-btn" onclick="avviaCicloAnalisi()">
        <span class="ring" aria-hidden="true"></span>
        <span id="btnLabel">Avvia scansione</span>
      </button>
      <div class="preview-frame" id="previewFrame">
        <img id="preview" alt="Ultima immagine acquisita dalla fotocamera">
      </div>
    </div>
  </section>

  <section>
    <div class="eyebrow">Registro eventi</div>
    <div class="panel log" id="log" role="log" aria-live="polite"></div>
  </section>
</div>

<script>
function scriviLog(testo, tipo){
  const logDiv=document.getElementById('log');
  const time=new Date().toLocaleTimeString('it-IT',{hour:'2-digit',minute:'2-digit',second:'2-digit'});
  const cls=tipo?(' '+tipo):'';
  logDiv.insertAdjacentHTML('beforeend','<div class="entry"><span class="t mono">'+time+'</span><span class="msg'+cls+'"></span></div>');
  logDiv.lastElementChild.querySelector('.msg').textContent = testo;
  logDiv.scrollTop=logDiv.scrollHeight;
}
scriviLog('Sistema inizializzato.');

const GAUGE_CIRC = 264;
async function aggiornaStatoBatteria(){
  try{
    let r=await fetch('/battery');
    let data=await r.json();
    document.getElementById('batVal').innerText=data.pct+'%';
    document.getElementById('chgIcon').innerText=data.charging?'⚡':'●';
    document.getElementById('batState').innerText=data.charging?'In carica':'A batteria';
    document.getElementById('batTime').innerText=data.time;
    let color = (data.charging || data.pct>20) ? 'var(--teal)' : 'var(--danger)';
    let arc=document.getElementById('gaugeArc');
    arc.style.stroke=color;
    let offset = GAUGE_CIRC - (GAUGE_CIRC*Math.min(data.pct,100)/100);
    arc.style.strokeDashoffset=offset;
  }catch(e){}
}
setInterval(aggiornaStatoBatteria,5000);
aggiornaStatoBatteria();

async function avviaCicloAnalisi(){
  const btn=document.getElementById('snapBtn');
  const label=document.getElementById('btnLabel');
  btn.disabled=true;
  btn.classList.add('scanning');
  label.innerText='Acquisizione...';
  scriviLog('Attivazione fotocamera...');
  try{
    let res=await fetch('/capture');
    if(!res.ok) throw new Error('Errore fotocamera');
    let base64Img=await res.text();
    const frame=document.getElementById('previewFrame');
    const img=document.getElementById('preview');
    img.src='data:image/jpeg;base64,'+base64Img;
    frame.style.display='block';

    label.innerText='Analisi in corso...';
    scriviLog('Invio al motore AI...');
    let resAi=await fetch('/analyze');
    let testoAi=await resAi.text();
    scriviLog(testoAi, 'ai');

    label.innerText='Riproduzione...';
    setTimeout(()=>{
      label.innerText='Avvia scansione';
      btn.disabled=false;
      btn.classList.remove('scanning');
    },2000);
  }catch(err){
    scriviLog('Errore: '+err.message,'err');
    label.innerText='Riprova';
    btn.disabled=false;
    btn.classList.remove('scanning');
  }
}
</script>
</body>
</html>
)HTMLPAGE";