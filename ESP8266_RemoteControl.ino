#include <ESP8266WiFi.h>
#include <ESPAsyncWebServer.h>
#include <ESPAsyncTCP.h>
#include <ArduinoJson.h>
// Define the WiFi username and password, and PARAM_dir parameter.
const char *ssid = "Mowwifi";
const char *password  = "123456789";
const char *PARAM_dir = "dir";
// Create an AsyncWebServer on port 80 to access the server.
AsyncWebServer server(80);
// Define an HTML page that contains the controls for car control.
// Here, HTML is used to create the UI interface and JavaScript is used to establish the function calls.
const char index_html[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html>
    <head>
        <link rel="shortcut icon" href="#" />
        <title>WiFi Control</title>
        <meta charset="UTF-8">
        <meta http-equiv="X-UA-Compatible" content="IE=edge">
        <meta name="viewport" content="width=device-width, initial-scale=1.0">
        <style>
            body {
                background-color: rgb(255, 255, 255);
                color: rgb(254, 174, 0);
                font-family: Arial;
                text-align: center;
                overflow: hidden;
            }
            .parent {
                display: flex;
                justify-content: center;
                align-items: center;
            }
            .joystick-container {
                margin: 3rem;
                display: inline-block;
            }
            .coordiv {
                border: 2px solid #ffbb00;
                border-radius: 0.9375rem;
            }
            #coordiv-left{
                position: absolute;
                left: 1%;
                top: 10%;
            }
            #coordiv-right{
                position: absolute;
                right: 1%;
                top: 20%;
            }
            #coordiv-all{
                margin-top: 140px;
                margin-left: 20px;
            }
            .slider-container {
                text-align: center;
            }
            .button-container {
                text-align: center;
            }
            .button {
                font-size: 1rem;
                border: none;
                border-radius: 0.5rem;
                background-color: #ffbb00;
                color: white;
                cursor: pointer;
            }
            .button.active {
                background-color: #ff3c00;
        </style>
    </head>
    <body>
        <div class="button-container">
            <button id="slowSpeed" class="button">slow</button>
            <button id="normalSpeed" class="button active">normal</button>
            <button id="fastSpeed" class="button">fast</button>
        </div>
        <div class="slider-container">
            <label for="speedSlider">Speed Controller: </label>
            <input type="range" min="0" max="20" value="0" class="slider" id="speedSlider">
        </div>
        <div class="joystick-container">
            <canvas id="coordiv-left" class="coordiv" width="100" height="200">do not support canvas</canvas>
            <div id="point-loc-left"></div>
        </div>
        <div class="joystick-container">
            <canvas id="coordiv-right" class="coordiv" width="200" height="100">do not support canvas</canvas>
        </div>
        <div class="joystick-container">
            <canvas id="coordiv-all" class="coordiv" width="200" height="200">do not support canvas</canvas>
        </div>
        <script>
            var VirtualJoystick	= function(opts)
            {
                opts			= opts			|| {};
                this._container		= opts.container	|| document.body;
                this._strokeStyle	= opts.strokeStyle	|| 'cyan';
                this._stickEl		= opts.stickElement	|| this._buildJoystickStick();
                this._baseEl		= opts.baseElement	|| this._buildJoystickBase();
                this._mouseSupport	= opts.mouseSupport !== undefined ? opts.mouseSupport : false;
                this._stationaryBase	= opts.stationaryBase || false;
                this._baseX		= this._stickX = opts.baseX || 0
                this._baseY		= this._stickY = opts.baseY || 0
                this._limitStickTravel	= opts.limitStickTravel || false
                this._stickRadius	= opts.stickRadius !== undefined ? opts.stickRadius : 100
                this._useCssTransform	= opts.useCssTransform !== undefined ? opts.useCssTransform : false
                this._direction = opts.direction || 'all'; // 'all', 'vertical', 'horizontal'
                this._region = opts.region || null; // Added region option
                this._container.style.position	= "relative"
                this._container.appendChild(this._baseEl)
                this._baseEl.style.position	= "absolute"
                this._baseEl.style.display	= "none"
                this._container.appendChild(this._stickEl)
                this._stickEl.style.position	= "absolute"
                this._stickEl.style.display	= "none"
                this._pressed	= false;
                this._touchIdx	= null;
                if(this._stationaryBase === true){
                    this._baseEl.style.display	= "";
                    this._baseEl.style.left		= (this._baseX - this._baseEl.width /2)+"px";
                    this._baseEl.style.top		= (this._baseY - this._baseEl.height/2)+"px";
                }
                this._transform	= this._useCssTransform ? this._getTransformProperty() : false;
                this._has3d	= this._check3D();               
                var __bind	= function(fn, me){ return function(){ return fn.apply(me, arguments); }; };
                this._$onTouchStart	= __bind(this._onTouchStart	, this);
                this._$onTouchEnd	= __bind(this._onTouchEnd	, this);
                this._$onTouchMove	= __bind(this._onTouchMove	, this);
                this._container.addEventListener( 'touchstart'	, this._$onTouchStart	, false );
                this._container.addEventListener( 'touchend'	, this._$onTouchEnd	, false );
                this._container.addEventListener( 'touchmove'	, this._$onTouchMove	, false );
                if( this._mouseSupport ){
                    this._$onMouseDown	= __bind(this._onMouseDown	, this);
                    this._$onMouseUp	= __bind(this._onMouseUp	, this);
                    this._$onMouseMove	= __bind(this._onMouseMove	, this);
                    this._container.addEventListener( 'mousedown'	, this._$onMouseDown	, false );
                    this._container.addEventListener( 'mouseup'	, this._$onMouseUp	, false );
                    this._container.addEventListener( 'mousemove'	, this._$onMouseMove	, false );
                }
            }
            VirtualJoystick.prototype.destroy	= function()
            {
                this._container.removeChild(this._baseEl);
                this._container.removeChild(this._stickEl);

                this._container.removeEventListener( 'touchstart'	, this._$onTouchStart	, false );
                this._container.removeEventListener( 'touchend'		, this._$onTouchEnd	, false );
                this._container.removeEventListener( 'touchmove'	, this._$onTouchMove	, false );
                if( this._mouseSupport ){
                    this._container.removeEventListener( 'mouseup'		, this._$onMouseUp	, false );
                    this._container.removeEventListener( 'mousedown'	, this._$onMouseDown	, false );
                    this._container.removeEventListener( 'mousemove'	, this._$onMouseMove	, false );
                }
            }
            /**
            * @returns {Boolean} true if touchscreen is currently available, false otherwise
            */
            VirtualJoystick.touchScreenAvailable	= function()
            {
                return 'createTouch' in document ? true : false;
            }
            /**
            * microevents.js - https://github.com/jeromeetienne/microevent.js
            */
            ;(function(destObj){
                destObj.addEventListener	= function(event, fct){
                    if(this._events === undefined) 	this._events	= {};
                    this._events[event] = this._events[event]	|| [];
                    this._events[event].push(fct);
                    return fct;
                };
                destObj.removeEventListener	= function(event, fct){
                    if(this._events === undefined) 	this._events	= {};
                    if( event in this._events === false  )	return;
                    this._events[event].splice(this._events[event].indexOf(fct), 1);
                };
                destObj.dispatchEvent		= function(event /* , args... */){
                    if(this._events === undefined) 	this._events	= {};
                    if( this._events[event] === undefined )	return;
                    var tmpArray	= this._events[event].slice(); 
                    for(var i = 0; i < tmpArray.length; i++){
                        var result	= tmpArray[i].apply(this, Array.prototype.slice.call(arguments, 1))
                        if( result !== undefined )	return result;
                    }
                    return undefined
                };
            })(VirtualJoystick.prototype);
            VirtualJoystick.prototype.deltaX	= function(){ return this._stickX - this._baseX;	}
            VirtualJoystick.prototype.deltaY	= function(){ return this._stickY - this._baseY;	}

            VirtualJoystick.prototype.up	= function(){
                if( this._pressed === false )	return false;
                var deltaX	= this.deltaX();
                var deltaY	= this.deltaY();
                if( deltaY >= 0 )				return false;
                if( Math.abs(deltaX) > 2*Math.abs(deltaY) )	return false;
                return true;
            }
            VirtualJoystick.prototype.down	= function(){
                if( this._pressed === false )	return false;
                var deltaX	= this.deltaX();
                var deltaY	= this.deltaY();
                if( deltaY <= 0 )				return false;
                if( Math.abs(deltaX) > 2*Math.abs(deltaY) )	return false;
                return true;	
            }
            VirtualJoystick.prototype.right	= function(){
                if( this._pressed === false )	return false;
                var deltaX	= this.deltaX();
                var deltaY	= this.deltaY();
                if( deltaX <= 0 )				return false;
                if( Math.abs(deltaY) > 2*Math.abs(deltaX) )	return false;
                return true;	
            }
            VirtualJoystick.prototype.left	= function(){
                if( this._pressed === false )	return false;
                var deltaX	= this.deltaX();
                var deltaY	= this.deltaY();
                if( deltaX >= 0 )				return false;
                if( Math.abs(deltaY) > 2*Math.abs(deltaX) )	return false;
                return true;	
            }
            VirtualJoystick.prototype._onUp	= function()
            {
                this._pressed	= false; 
                this._stickEl.style.display	= "none";
                
                if(this._stationaryBase == false){	
                    this._baseEl.style.display	= "none";
                
                    this._baseX	= this._baseY	= 0;
                    this._stickX	= this._stickY	= 0;
                }
            }

            VirtualJoystick.prototype._onDown	= function(x, y)
            {
                this._pressed	= true; 
                if(this._stationaryBase == false){
                    this._baseX	= x;
                    this._baseY	= y;
                    this._baseEl.style.display	= "";
                    this._move(this._baseEl.style, (this._baseX - this._baseEl.width /2), (this._baseY - this._baseEl.height/2));
                }
                this._stickX	= x;
                this._stickY	= y;
                if(this._limitStickTravel === true){
                  var deltaX	= this.deltaX();
                  var deltaY	= this.deltaY();
                  var stickDistance = Math.sqrt( (deltaX * deltaX) + (deltaY * deltaY) );
                  if(stickDistance > this._stickRadius){
                      var stickNormalizedX = deltaX / stickDistance;
                      var stickNormalizedY = deltaY / stickDistance;
                      
                      this._stickX = stickNormalizedX * this._stickRadius + this._baseX;
                      this._stickY = stickNormalizedY * this._stickRadius + this._baseY;
                  } 	
                }
                this._stickEl.style.display	= "";
                this._move(this._stickEl.style, (this._stickX - this._stickEl.width /2), (this._stickY - this._stickEl.height/2));	
            }

            VirtualJoystick.prototype._onMove	= function(x, y)
            {
                if( this._pressed === true ){
                    if(this._direction === 'vertical') {
                        this._stickY = y;
                    } else if(this._direction === 'horizontal') {
                        this._stickX = x;
                    } else {
                        this._stickX = x;
                        this._stickY = y;
                    }
                    
                    if(this._limitStickTravel === true){
                        var deltaX	= this.deltaX();
                        var deltaY	= this.deltaY();
                        var stickDistance = Math.sqrt( (deltaX * deltaX) + (deltaY * deltaY) );
                        if(stickDistance > this._stickRadius){
                            var stickNormalizedX = deltaX / stickDistance;
                            var stickNormalizedY = deltaY / stickDistance;
                        
                            if(this._direction === 'vertical') {
                                this._stickY = stickNormalizedY * this._stickRadius + this._baseY;
                            } else if(this._direction === 'horizontal') {
                                this._stickX = stickNormalizedX * this._stickRadius + this._baseX;
                            } else {
                                this._stickX = stickNormalizedX * this._stickRadius + this._baseX;
                                this._stickY = stickNormalizedY * this._stickRadius + this._baseY;
                            }
                        } 		
                    }
                    
                        this._move(this._stickEl.style, (this._stickX - this._stickEl.width /2), (this._stickY - this._stickEl.height/2));	
                }	
            }
            VirtualJoystick.prototype._onMouseUp	= function(event)
            {
                return this._onUp();
            }

            VirtualJoystick.prototype._onMouseDown	= function(event)
            {
                if (this._region && !this._region.contains(event.clientX, event.clientY)) return;
                var x	= event.clientX;
                var y	= event.clientY;
                return this._onDown(x, y);
            }

            VirtualJoystick.prototype._onMouseMove	= function(event)
            {
                var x	= event.clientX;
                var y	= event.clientY;
                return this._onMove(x, y);
            }
            VirtualJoystick.prototype._onTouchStart	= function(event)
            {
                // if there is already a touch inprogress do nothing
                if( this._touchIdx !== null )	return;

                // notify event for validation
                var isValid	= this.dispatchEvent('touchStartValidation', event);
                if( isValid === false )	return;
                
                if (this._region && !this._region.contains(event.changedTouches[0].pageX, event.changedTouches[0].pageY)) return;
                // dispatch touchStart
                this.dispatchEvent('touchStart', event);

 
                // get the first who changed
                var touch	= event.changedTouches[0];
                // set the touchIdx of this joystick
                this._touchIdx	= touch.identifier;

                // forward the action
                var x		= touch.pageX;
                var y		= touch.pageY;
                return this._onDown(x, y)
            }

            VirtualJoystick.prototype._onTouchEnd	= function(event)
            {
                // if there is no touch in progress, do nothing
                if( this._touchIdx === null )	return;

                // dispatch touchEnd
                this.dispatchEvent('touchEnd', event);

                // try to find our touch event
                var touchList	= event.changedTouches;
                for(var i = 0; i < touchList.length && touchList[i].identifier !== this._touchIdx; i++);
                // if touch event isnt found, 
                if( i === touchList.length)	return;

                // reset touchIdx - mark it as no-touch-in-progress
                this._touchIdx	= null;
            event.preventDefault();

                return this._onUp()
            }

            VirtualJoystick.prototype._onTouchMove	= function(event)
            {
                // if there is no touch in progress, do nothing
                if( this._touchIdx === null )	return;

                // dispatch touchMove
                this.dispatchEvent('touchMove', event);

                // try to find our touch event
                var touchList	= event.changedTouches;
                /* if (this._region && !this._region.contains(event.changedTouches[0].pageX, event.changedTouches[0].pageY)) return; */
                for(var i = 0; i < touchList.length && touchList[i].identifier !== this._touchIdx; i++ );
                // if touch event with the proper identifier isnt found, do nothing
                if( i === touchList.length)	return;
                var touch	= touchList[i];


                var x		= touch.pageX;
                var y		= touch.pageY;
                return this._onMove(x, y)
            }
            /**
            * build the canvas for joystick base
            */
            VirtualJoystick.prototype._buildJoystickBase	= function()
            {
                var canvas	= document.createElement( 'canvas' );
                canvas.width	= 126;
                canvas.height	= 126;
                
                var ctx		= canvas.getContext('2d');
                ctx.beginPath(); 
                ctx.strokeStyle = this._strokeStyle; 
                ctx.lineWidth	= 6; 
                ctx.arc( canvas.width/2, canvas.width/2, 40, 0, Math.PI*2, true); 
                ctx.stroke();	

                ctx.beginPath(); 
                ctx.strokeStyle	= this._strokeStyle; 
                ctx.lineWidth	= 2; 
                ctx.arc( canvas.width/2, canvas.width/2, 60, 0, Math.PI*2, true); 
                ctx.stroke();
                
                return canvas;
            }

            /**
            * build the canvas for joystick stick
            */
            VirtualJoystick.prototype._buildJoystickStick	= function()
            {
                var canvas	= document.createElement( 'canvas' );
                canvas.width	= 86;
                canvas.height	= 86;
                var ctx		= canvas.getContext('2d');
                ctx.beginPath(); 
                ctx.strokeStyle	= this._strokeStyle; 
                ctx.lineWidth	= 6; 
                ctx.arc( canvas.width/2, canvas.width/2, 40, 0, Math.PI*2, true); 
                ctx.stroke();
                return canvas;
            }
            VirtualJoystick.prototype._move = function(style, x, y)
            {
                if (this._transform) {
                    if (this._has3d) {
                        style[this._transform] = 'translate3d(' + x + 'px,' + y + 'px, 0)';
                    } else {
                        style[this._transform] = 'translate(' + x + 'px,' + y + 'px)';
                    }
                } else {
                    style.left = x + 'px';
                    style.top = y + 'px';
                }
            }

            VirtualJoystick.prototype._getTransformProperty = function() 
            {
                var styles = [
                    'webkitTransform',
                    'MozTransform',
                    'msTransform',
                    'OTransform',
                    'transform'
                ];

                var el = document.createElement('p');
                var style;

                for (var i = 0; i < styles.length; i++) {
                    style = styles[i];
                    if (null != el.style[style]) {
                        return style;
                    }
                }         
            }
            
            VirtualJoystick.prototype._check3D = function() 
            {        
                var prop = this._getTransformProperty();
                // IE8<= doesn't have `getComputedStyle`
                if (!prop || !window.getComputedStyle) return module.exports = false;

                var map = {
                    webkitTransform: '-webkit-transform',
                    OTransform: '-o-transform',
                    msTransform: '-ms-transform',
                    MozTransform: '-moz-transform',
                    transform: 'transform'
                };
                var el = document.createElement('div');
                el.style[prop] = 'translate3d(1px,1px,1px)';
                document.body.insertBefore(el, null);
                var val = getComputedStyle(el).getPropertyValue(map[prop]);
                document.body.removeChild(el);
                var exports = null != val && val.length && 'none' != val;
                return exports;
            }

            class Region {
                constructor(x, y, width, height) {
                    this.x = x;
                    this.y = y;
                    this.width = width;
                    this.height = height;
                }
                
                contains(x, y) {
                    return x >= this.x && x <= this.x + this.width &&
                        y >= this.y && y <= this.y + this.height;
                }
            }


            document.body.style.overflow = 'hidden';
            window.addEventListener('wheel', function(e) {
                e.preventDefault();
            }, { passive: false });
            window.addEventListener('touchmove', function(e) {
                e.preventDefault();
            }, { passive: false });

            let speedCar = 0;
            let rotation = 0;
            let direction = 'S';
            let middleJoystick = { x: 150 , y: 150 };
            const leftCanvas = document.getElementById('coordiv-left');
            const rightCanvas = document.getElementById('coordiv-right');
            const middleCanvas = document.getElementById('coordiv-all')
            const leftCtx = leftCanvas.getContext('2d');
            const rightCtx = rightCanvas.getContext('2d');
            const middleCtx = middleCanvas.getContext('2d');
            const leftCenter = { x: leftCanvas.width / 2, y: leftCanvas.height / 2 };
            const rightCenter = { x: rightCanvas.width / 2, y: rightCanvas.height / 2 };
            const middleCenter = { x: middleCanvas.width / 2, y: middleCanvas.height / 2 }

            const speedSlider = document.getElementById('speedSlider');
            const slowSpeedBtn = document.getElementById('slowSpeed');
            const normalSpeedBtn = document.getElementById('normalSpeed');
            const fastSpeedBtn = document.getElementById('fastSpeed');
            let speedMode = 1;
            const setSpeedMode = (mode) => {
                if (speedMode === mode) {
                    speedMode = 0; // Reset to zero if the same button is pressed again
                    slowSpeedBtn.classList.remove('active');
                    normalSpeedBtn.classList.remove('active');
                    fastSpeedBtn.classList.remove('active');
                } else {
                    speedMode = mode;
                    slowSpeedBtn.classList.remove('active');
                    normalSpeedBtn.classList.remove('active');
                    fastSpeedBtn.classList.remove('active');
                    if (mode === 0.5) {
                        slowSpeedBtn.classList.add('active');
                    } else if (mode === 1) {
                        normalSpeedBtn.classList.add('active');
                    } else if (mode === 2) {
                        fastSpeedBtn.classList.add('active');
                    }
                }
            };
            slowSpeedBtn.addEventListener('click', () => setSpeedMode(0.5));
            normalSpeedBtn.addEventListener('click', () => setSpeedMode(1));
            fastSpeedBtn.addEventListener('click', () => setSpeedMode(2));


            function getCanvasCenter(canvas) {
                const rect = canvas.getBoundingClientRect();
                const centerX = rect.left + rect.width/2
                const centerY = rect.top + rect.height/2
                return { x: centerX, y: centerY };
            }
            function updateJoystickRegions() {
            var leftRegion = new Region(0, 0, leftCanvas.width, leftCanvas.height);
            var rightRegion = new Region(getCanvasCenter(rightCanvas).x - rightCanvas.width/2, 80, rightCanvas.width, rightCanvas.height);
			
            var leftjoystick	= new VirtualJoystick({
				container	: document.body,
				strokeStyle	: 'orange',
				limitStickTravel: true,
				stickRadius	: 100,
                direction: 'vertical',
                region:leftRegion,

			});
			leftjoystick.addEventListener('touchStartValidation', function(event){
				var touch	= event.changedTouches[0];
				if( touch.pageX >= window.innerWidth/2 )	return false;
				return true
			});
			leftjoystick.addEventListener('touchMove', function(){
                let position = -leftjoystick.deltaY()
                // Normalize position to range 0-20 for speedSlider
                let sliderValue = Math.abs(position/100 * 20);
                speedSlider.value = sliderValue;
                speedCar = sliderValue*speedMode;
                if (position > 0) {
                    direction = 'F';
                    speedCar = position/100*speedMode;
                } else if (position < 0) {
                    direction = 'B';
                    speedCar = position/100*speedMode;
                } else {
                    direction = 'S';
                    speedCar = 0;
                }
			})

            leftjoystick.addEventListener('touchEnd', function(){
                let sliderValue = 0;
                direction = 'S';
                speedSlider.value = sliderValue;
                speedCar = sliderValue*speedMode;
			})  

            var rightjoystick	= new VirtualJoystick({
				container	: document.body,
				strokeStyle	: 'cyan',
				limitStickTravel: true,
				stickRadius	: 100,
                direction: 'horizontal',
                region: rightRegion,
			});
			rightjoystick.addEventListener('touchStartValidation', function(event){
				var touch	= event.changedTouches[0];
				if( touch.pageX < window.innerWidth/2 )	return false;
				return true
			});
			rightjoystick.addEventListener('touchMove', function(){
                let position = rightjoystick.deltaX();
                if (position < 0) {
                    direction = 'L';
                    rotation = position/100;
                } else if (position > 0) {
                    direction = 'R';
                    rotation = position/100;
                } else {
                    direction = 'S';
                    rotation = 0;
                }
            });
            rightjoystick.addEventListener('touchEnd', function(){
                direction = 'S';
                rotation = 0;
			})  
            }
            
            const updateJoystick = function(canvas, ctx, center, joystick, locId, isLeft){
                ctx.clearRect(0, 0, canvas.width, canvas.height);
                ctx.save();
                ctx.beginPath();
                ctx.arc(center.x, center.y, 100, 0, 2 * Math.PI, false);
                ctx.fillStyle = "#CCCCCC";
                ctx.fill();
                ctx.shadowBlur = 10;
                ctx.shadowColor = "#ff9900";
                ctx.strokeStyle = "#333333";
                ctx.stroke();

                // Constrain joystick within base circle
                let dx = joystick.x - center.x;
                let dy = joystick.y - center.y;
                let distance = Math.sqrt(dx * dx + dy * dy);
                if (distance > 100) {
                    dx *= 100 / distance;
                    dy *= 100 / distance;
                    joystick.x = center.x + dx;
                    joystick.y = center.y + dy;
                }

                ctx.beginPath();
                ctx.arc(joystick.x, joystick.y, 200 / 4, 0, 2 * Math.PI, false);
                ctx.fillStyle = "#21242d";
                ctx.fill();
                ctx.stroke();
                ctx.closePath();
                ctx.restore();

                //const loc = `current position ${isLeft ? 'y' : 'x'}: ${parseInt((isLeft ? joystick.y : joystick.x) / (center.x / 5))}`;
                //document.getElementById(locId).innerHTML = loc;

                if (isLeft) {
                    let speedValue = -parseInt((joystick.y - center.y));
                    if (speedValue < 0) {
                        direction = 'B';
                        speedCar = speedValue/100*speedMode;
                    } else if (speedValue > 0) {
                        direction = 'F';
                        speedCar = speedValue/100*speedMode;
                    } else {
                        direction = 'S';
                        speedCar = 0;
                    }
                    speedSlider.value = Math.abs(speedValue/100 *20);
                    let steeringValue = (joystick.x - center.x) / 100;
                    if (steeringValue < 0) {
                        direction = 'L';
                        rotation = steeringValue;
                    } else if (steeringValue > 0) {
                        direction = 'R';
                        rotation = steeringValue;
                    } else {
                        direction = 'S';
                        rotation = 0;
                    }
                }
            }

            let initJoystick = function(canvas, ctx, center, joystick, locId, isLeft) {
                const activeTouches = {};
                const getTouch = (e) => e.touches.length ? e.touches[0] : e.changedTouches[0];
                
                const handleMovement = (e, touch) => {
                    const x = canvas.offsetLeft;
                    const y = canvas.offsetTop;
                    let NowX = touch ? touch.clientX - x : e.clientX - x;
                    let NowY = touch ? touch.clientY - y : e.clientY - y;
                    
                    NowX = Math.max(0, Math.min(NowX, canvas.width));
                    NowY = Math.max(0, Math.min(NowY, canvas.height));
                    
                    joystick.x = NowX;
                    joystick.y = NowY;
                    
                    updateJoystick(canvas, ctx, center, joystick, locId, isLeft);
                };
                
                canvas.addEventListener('mousedown', function(e) {
                    handleMovement(e);
                    
                    const mouseMoveHandler = (e) => handleMovement(e);
                    const mouseUpHandler = () => {
                        joystick.x = center.x;
                        joystick.y = center.y;
                        updateJoystick(canvas, ctx, center, joystick, locId, isLeft);
                        document.removeEventListener('mousemove', mouseMoveHandler);
                        document.removeEventListener('mouseup', mouseUpHandler);
                    };
                    
                    document.addEventListener('mousemove', mouseMoveHandler);
                    document.addEventListener('mouseup', mouseUpHandler);
                });

                canvas.addEventListener('touchstart', function(e) {
                    const touch = getTouch(e);
                    handleMovement(e, touch);
                    activeTouches[touch.identifier] = touch;
                    
                    const touchMoveHandler = (e) => {
                        for (let touch of e.touches) {
                            if (activeTouches[touch.identifier]) {
                                handleMovement(e, touch);
                            }
                        }
                    };

                    const touchEndHandler = (e) => {
                        for (let touch of e.changedTouches) {
                            if (activeTouches[touch.identifier]) {
                                joystick.x = center.x;
                                joystick.y = center.y;
                                updateJoystick(canvas, ctx, center, joystick, locId, isLeft);
                                delete activeTouches[touch.identifier];
                            }
                        }
                    };
                    
                    canvas.addEventListener('touchmove', touchMoveHandler);
                    canvas.addEventListener('touchend', touchEndHandler);
                });
            };
            initJoystick(middleCanvas, middleCtx, middleCenter, middleJoystick, 'point-loc-left', true);
            window.onload = updateJoystickRegions;
            window.onresize = updateJoystickRegions;
            
            let loop = setInterval(function(){
                    SentCommand(rotation,speedCar);
                }, 100);

            let SentCommand = function(angle, speed){
                let xhr = new XMLHttpRequest();
                xhr.open("GET", "/direction?rotation=" + angle + "&speedCar=" + speed, true);
                xhr.send();
            }
        </script>
    </body>
</html> 
)rawliteral";

// Next, WiFi.begin is used to connect to the WiFi network and wait for a successful connection.
// Finally, the server route is set up to respond to HTTP GET requests to the root path "/" and return an HTML page.
void setup(){
    Serial.begin(115200);
    bool ssid_hidden = false; // Set to true to hide the SSID
    int channel = 1;          // Set the WiFi channel
    int max_connection = 4;   // Set the maximum number of connections
    // Set the ESP8266 to AP mode using the defined SSID and password
    bool result = WiFi.softAP(ssid, password, channel, ssid_hidden, max_connection);
    Serial.println("ESP8266 is set as a WiFi Access Point with SSID: " + String(ssid));
    if (result)
    {
        Serial.println("Access Point Started");
        IPAddress IP = WiFi.softAPIP();
        Serial.print("AP IP address: ");
        Serial.println(IP);
    }
    else
    {
        Serial.println("Access Point Failed to Start");
    }
    // Set up the server route to respond to HTTP GET requests to the root path "/" and return an HTML page.
    server.on("/", HTTP_GET, [](AsyncWebServerRequest *request) {
        request->send_P(200, "text/html", index_html);
        Serial.print("get success!");
    }); 
    // On the server object, set up an HTTP GET route named "direction" that uses the getParam function of the request object to get the values of speedCar and dir parameters,
    // which represent the speed and direction of the car, respectively.
    server.on("/direction", HTTP_GET, [](AsyncWebServerRequest *request) {
        float speedCar =0;
        float rotation = 0; 
        if(request->hasParam("speedCar")){
            speedCar = request->getParam("speedCar")->value().toFloat();
        }
        if(request->hasParam("rotation")){
            rotation = request->getParam("rotation")->value().toFloat();
        }
        handleDirection(rotation,speedCar);
        request->send(200, "text/html","receive rotation:" + String(rotation) + "receive linear speed:" + String(speedCar)) ;
    });
    server.begin();
}
void handleDirection(float rotation, float speedCar){
    byte data[sizeof(float) * 2 + 1]; 
    memcpy(data, &rotation, sizeof(float));
    data[sizeof(float)] = 0xFF;
    memcpy(data + sizeof(float) + 1, &speedCar, sizeof(float));
    Serial.print("") // Something for debugging
    Serial.write(data, sizeof(data));
    
}
void loop(){
}
