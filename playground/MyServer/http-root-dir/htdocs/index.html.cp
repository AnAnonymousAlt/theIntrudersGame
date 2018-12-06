<!DOCTYPE HTML PUBLIC "-//IETF//DTD HTML//EN">
<html>
  <head>
    <title>CS422: HTTP Server</title>
  
  <style>
  canvas {
    border: 1px solid #d3d3d3;
    background-color: #f1f1f1;
  }
  </style>
  </head>
  <body onload="startGame()">
    <script>
    function startGame() {
      myGameArea.start();
    }
    var myGameArea = {
      canvas : document.createElement("canvas"),
      start : functions() {
        this.canvas.width = 480;
        this.canvas.height = 270;
        this.context = this.canvas.getContext("2d");
        document.body.insertBefore(this.canvas, document.body.childNodes[0]);
      }
    }
    <h1>CS252: HTTP Server</h1>
    <link href="main.css" rel="stylesheet" type="text/css" />

    <ul>
    <li><A HREF="simple.html"> Simple Test</A>
    <li><A HREF="complex.html"> Complex Test</A>
    <li><A HREF="dir1/"> Browsing dir1/</A>
    <li><A HREF="dir1"> Browsing dir</A>
    <li><A HREF="/cgi-bin/test-env"> cgi-bin: test-env</A>
    <li><A HREF="/cgi-bin/test-cgi"> cgi-bin: test-cgi</A>
    <li><A HREF="/cgi-bin/finger"> cgi-bin: finger</A>
    <li><A HREF="/cgi-bin/jj"> cgi-bin: pizza</A>
    <li><A HREF="/cgi-bin/hello.so?a=b"> cgi-bin: hello.so</A>
    <li><A HREF="/cgi-bin/jj-mod.so?a=b"> cgi-bin: jj-mod.so</A>
    <li><A HREF="/logs"> logs</A>
    <li><A HREF="/stats"> stats</A>
    </ul>

    <hr>
    <address><a href="mailto:grr@cs.purdue.edu">Gustavo Rodriguez-Rivera</a></address>
<!-- Created: Fri Dec  5 13:12:48 EST 1997 -->
<!-- hhmts start -->
Last modified: Fri Dec  5 13:14:47 EST 1997
<!-- hhmts end -->
  </body>
</html>
