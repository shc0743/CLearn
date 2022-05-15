((function(){
    window.addEventListener('DOMContentLoaded', function () {
        let iframe = document.querySelector('iframe') || {};
        iframe.tabIndex = 0;

        let protname = window.__protname;
        let evid = window.__eventid;
        if (!(protname)) return false;

        let accept_dialog = document.createElement('div');
        accept_dialog.id = 'accept_dialog';
        accept_dialog.innerHTML =
            "<button class=\"accept\">Accept</button>" + 
            "<button class=\"decline\">Decline</button>";
        document.body.appendChild(accept_dialog);

        let window_onresize = function () {
            iframe.style.height = 
                document.documentElement.clientHeight - 
                accept_dialog.offsetHeight + 'px';
        }
        window.addEventListener('resize', window_onresize);
        window_onresize();

        let 
            accept = accept_dialog.querySelector('.accept') || {},
            decline = accept_dialog.querySelector('.decline') || {};

        if (window.__dwTimesToAccept) {
            accept.disabled = true;
            let dwTime = window.__dwTimesToAccept;
            let nId = 0, nCount = 0;
            accept.dataset.n = dwTime;
            nId = setInterval(function () {
                if ((++nCount) >= dwTime) {
                    accept.removeAttribute('data-n');
                    accept.disabled = false;
                    clearInterval(nId);
                } else {
                    accept.dataset.n = dwTime - nCount;
                }

            }, 1000);
        }

        accept.onclick = function () {
            let w = window.open(protname + '://accept/' + evid, '_blank');
            w.opener = null;
            window.close();
        }
        decline.onclick = function () {
            let w = window.open(protname + '://decline/' + evid, '_blank');
            w.opener = null;
            window.close();
        }
    });
})())
