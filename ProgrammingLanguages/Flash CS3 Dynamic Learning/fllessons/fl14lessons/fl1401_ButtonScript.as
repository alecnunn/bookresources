stop();
function playMovie(event:Event):void{
	this.play();
}
function pauseMovie(event:Event):void{
	this.stop();
}
function stopMovie(event:Event):void{
	this.gotoAndPlay(1);
}
play_btn.addEventListener(MouseEvent.CLICK,playMovie);
pause_btn.addEventListener(MouseEvent.CLICK,pauseMovie);
stop_btn.addEventListener(MouseEvent.CLICK,stopMovie);