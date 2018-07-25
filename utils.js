tb = {};

tb.init = function() {
    tb.output = document.getElementById('output');
    tb.tick = Module.cwrap('tick', 'void', ['number']);
    tb.initCore = Module.cwrap('initCore', 'void', []);
    tb.initCore();
    tb.buf = [];
    document.onkeydown = tb.key;
}

tb.putc = function(c) {
    if (c == 8) {
        if (tb.output.value.length > 0) {
            tb.output.value = tb.output.value.substring(0, tb.output.value.length - 1);
        }
    } else {
        tb.output.value += String.fromCharCode(c);
    }
}

tb.key = function(e) {
    var k = e.key;
    var c = -1;
    if (k.length == 1) {
        c = k.charCodeAt(0);
    } else if (k == 'Enter') {
        c = 13;
    } else if (k == 'Backspace') {
        c = 8;
    }
    if (c >= 0) {
        tb.buf.push(c);
    }
}

tb.dispatch = function() {
    tb.tick(tb.buf.length > 0 ? tb.buf.shift() : -1)
}

tb.init();
setInterval('tb.dispatch()', 100);

