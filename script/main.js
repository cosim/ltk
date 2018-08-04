for (var k in win32) {
	print(k, " = ", win32[k]);
}

var wnd = new Window();
wnd.Create({
	style : win32.WS_OVERLAPEDWINDOW,
	rect : {x : 0, y : 0, w : 800, h : 600}
});
wnd.Show(win32.SW_SHOWNOACTIVATE);
