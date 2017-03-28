/****************************
 * Slider theme for Gallery2
 * @author Alan Harder <alan.harder@sun.com>
 * $Revision: 16920 $
 */

//Class app
var app_ww, app_wh, // Window width/height
    app_agent = navigator.userAgent.toLowerCase(), // Browser type
    app_version = parseInt(navigator.appVersion),
    app_is_ie = app_agent.indexOf('msie') >= 0 && app_agent.indexOf('opera') < 0,
    app_is_opera = app_agent.indexOf('opera') >= 0,
    app_is_safari = app_agent.indexOf('safari') >= 0,
    image_area, image_div, title_div, thumb_div; // Containers
if (window.attachEvent) {
  window.attachEvent('onunload', app_setcookie);
} else if (window.addEventListener) {
  window.addEventListener('unload', app_setcookie, false);
  if (app_is_safari) window.addEventListener('load', app_onload, false);
}
function app_init() {
  image_area = document.getElementById('imagearea');
  image_div = document.getElementById('image');
  title_div = document.getElementById('titlebar');
  thumb_div = document.getElementById('thumbs');
  ui_vis('opts', 1, 1);
  ui_vis('slide_fwd', 1, 1);

  document.onkeypress = app_onkeypress;
  if (window.attachEvent) window.attachEvent('onresize', app_onresize);
  else if (window.addEventListener) window.addEventListener('resize', app_onresize, false);

  if (app_is_ie) {
    image_area.style.width = title_div.style.width = '100%';
    thumb_div.style.overflow = 'scroll';
    document.body.parentNode.style.overflow = 'hidden';
    document.onkeydown = app_onkeydown;
  } else if (app_is_opera) {
    document.getElementById('options').style.width = '180px';
  }

  app_getwinsize();
  app_getcookie();
  if (data_count > 0) image_show(data_view);
}
function app_getwinsize() {
  if (typeof(window.innerWidth) == 'number') {
    app_ww = window.innerWidth; app_wh = window.innerHeight;
  } else if (document.documentElement.clientHeight) {
    app_ww = document.documentElement.clientWidth; app_wh = document.documentElement.clientHeight;
  } else {
    app_ww = document.body.clientWidth; app_wh = document.body.clientHeight;
  }
}
function app_onresize() {
  app_getwinsize();
  if (app_is_ie) {
    image_setsize();
    if (options_on) options_setsize();
    if (!thumbs_horiz) {
      thumb_div.style.height = app_wh + 'px';
      image_area.style.width = title_div.style.width = (app_ww - thumb_div.offsetWidth) + 'px';
    }
  } else if (app_is_opera && !thumbs_horiz) thumb_div.style.height = app_wh + 'px';
  image_fit();
}
function app_onload() {
  image_setsize();
  image_fit();
}
function app_setcookie() {
  var d = new Date(), c = thumbs_horiz + ';' + slide_order + ';' + (slide_delay/1000) + ';';
  d.setTime(d.getTime() + 90*24*60*60*1000); // 90 day cookie
  document.cookie = 'G2_slider=' + escape(c) + ';expires=' + d.toUTCString();
}
function app_getcookie() {
  var c = getcookie('G2_slider'), i,j,v,n,it=1;
  if (c) {
    for (i=0, j = c.indexOf(';', 0); j >= 0; i = j+1, j = c.indexOf(';', i)) {
      v = c.substring(i,j);
      n = parseInt(v);
      switch (it++) {
	case 1: if (n==0) thumbs_horizvert(); break;
	case 2: ui_select('slide_order', n); slide_setorder(n); break;
	case 3: ui_select('slide_delay', n); slide_setdelay(n); break;
      }
    }
  }
}
function app_onkeypress(event) {
  if (app_is_ie) event = window.event; //For IE
  var keyCode = event.keyCode ? event.keyCode : event.which;
  if (event.ctrlKey || event.altKey || event.metaKey) return;
  if (app_is_safari) switch (keyCode) { //For Safari
    case 63232: keyCode=38; break;  case 63273: keyCode=36; break;
    case 63233: keyCode=40; break;  case 63275: keyCode=35; break;
    case 63234: keyCode=37; break;  case 63276: keyCode=33; break;
    case 63235: keyCode=39; break;  case 63277: keyCode=34; break;
  }
  /* space = start/pause slideshow
   * page up/down, home/end = scroll thumbnails
   * left/right = next/prev image
   * up/down = full/fit size
   * arrow keys scroll image in full size; use shift-arrows for funcs above
   */
  if (keyCode==32) slide_onoff();
  if (keyCode < 33 || keyCode > 40) return;
  if (event.shiftKey) keyCode += 100;
  switch (keyCode) {
    case 37: //Left
      if (image_zoomon) { image_area.scrollLeft -= 20; break; }
    case 137: //Shift-Left
      image_prev();
      break;
    case 39: //Right
      if (image_zoomon) { image_area.scrollLeft += 20; break; }
    case 139: //Shift-Right
      image_next();
      break;
    case 38: //Up
      if (image_zoomon) { image_area.scrollTop -= 20; break; }
    case 138: //Shift-Up
      if (document.getElementById('full_size').style.display == 'inline') image_zoom(1);
      break;
    case 40: //Down
      if (image_zoomon) { image_area.scrollTop += 20; break; }
    case 140: //Shift-Down
      if (document.getElementById('fit_size').style.display == 'inline') image_zoom(0);
      break;
    case 33: //PageUp
      if (thumbs_horiz) thumb_div.scrollLeft -= app_ww - 10;
		   else thumb_div.scrollTop -= app_wh - 10;
      break;
    case 34: //PageDown
      if (thumbs_horiz) thumb_div.scrollLeft += app_ww - 10;
		   else thumb_div.scrollTop += app_wh - 10;
      break;
    case 36: //Home
      if (thumbs_horiz) thumb_div.scrollLeft = 0;
		   else thumb_div.scrollTop = 0;
      break;
    case 35: //End
      if (thumbs_horiz) thumb_div.scrollLeft = thumb_div.scrollWidth;
		   else thumb_div.scrollTop = thumb_div.scrollHeight;
      break;
  }
}
function app_onkeydown() {
  if (window.event.keyCode >= 32 && window.event.keyCode <= 40) {
    app_onkeypress(); window.event.returnValue = false;
  }
}

//Class image :: div imagearea(image)
var image_index=0, // Index of visible image
    image_zoomon=0, // Image is zoomed to full size
    image_cache = new Image, // For precaching an image
    image_iscached = new Array(data_count); // Track precached images
function image_setsize() {
  image_area.style.bottom = title_div.offsetHeight + 'px';
  if (app_is_ie) image_area.style.height = (app_wh - title_div.offsetHeight) + 'px';
}
function image_show(i) {
  slide_reset();
  image_index = i;
  ui_sethtml('title', document.getElementById('title_'+image_index).innerHTML);
  image_setsize();
  if (options_on) { options_setsize(); options_setItemLinks(i); }
  if (image_map && app_is_ie) {
    document.getElementById('prevArrow').style.visibility = 'hidden';
    document.getElementById('nextArrow').style.visibility = 'hidden';
  }
  if (data_iw[i] < 0) {
    image_div.innerHTML = '<iframe style="width:100%;height:' + (image_area.offsetHeight-4)
      + 'px" frameborder="0" src="' + document.getElementById('img_'+i).href + '"></iframe>';
    ui_vis('fit_size', 0); ui_vis('full_size', 0);
  } else {
    var s = image_fit(1);
    image_div.innerHTML = '<img name="slide" src="' + document.getElementById('img_'+i).href
      + '" ' + s + ' onload="image_loaded()" alt=""' + image_map + '/>';
  }
  image_setbuttons();
  if (slide_inprog && !slide_on) slide_inprog = 0;
}
function image_fit(getstr) {
  if (!getstr && !document.slide) return;
  var w = data_iw[image_index], h = data_ih[image_index],
      aw = image_area.offsetWidth, ah = image_area.offsetHeight, a=0;
  if (w > aw || h > ah) {
    if ((a = h/w) < ah/aw) {
      w = aw; h = Math.round(aw*a);
    } else {
      w = Math.round(ah/a); h = ah;
    }
  }
  if (image_zoomon) {
    if (getstr || !a) image_zoom(0,1);
    else { image_zoom(1); return; }
  }
  image_div.style.paddingTop = Math.floor((ah-h)/2) + 'px';
  ui_vis('fit_size', 0);
  ui_vis('full_size', a, 1);
  if (image_map) set_image_map(w,h,aw-w);
  if (getstr) {
    return 'width="' + w + '" height="' + h + '"';
  } else {
    document.slide.width = w;
    document.slide.height = h;
  }
}
function image_zoom(on,noresize) {
  image_area.style.overflow = on ? 'auto' : 'hidden';
  if (!on) image_area.scrollLeft = image_area.scrollTop = 0;
  if (image_zoomon=on) {
    ui_vis('full_size', 0);
    ui_vis('fit_size', 1, 1);
    var h = image_area.offsetHeight - data_ih[image_index];
    image_div.style.paddingTop = max(Math.floor(h/2), 0) + 'px';
    document.slide.width = data_iw[image_index];
    document.slide.height = data_ih[image_index];
    if (image_map) set_image_map(data_iw[image_index], data_ih[image_index],
				 image_area.offsetWidth - data_iw[image_index]);
  } else if (!noresize) { image_fit(); }
}
function image_precache(i) {
  if (!image_iscached[i]) {
    image_iscached[i] = 1;
    image_cache.src = document.getElementById('img_'+i).href;
  }
}
function image_loaded() {
  var i = slide_nextindex(); if (i >= 0) image_precache(i);
  slide_go(i);
}
function image_next() {
  var i = slide_nextindex(); if (i >= 0) image_show(i);
}
function image_prev() {
  var i = slide_previndex(); if (i >= 0) image_show(i);
}
function image_setbuttons() {
  var i = slide_nextindex(), j = slide_previndex();
  ui_vis('next_img', i >= 0, 1);
  ui_vis('next_off', i < 0, 1);
  ui_vis('prev_img', j >= 0, 1);
  ui_vis('prev_off', j < 0, 1);
}
function set_image_map(w,h,ww) {
  var map = document.getElementById('prevnext'),
      pa = document.getElementById('prevArrow'), na = document.getElementById('nextArrow'),
      sw = max(Math.floor(ww/2),0) + 30, i = slide_nextindex(), j = slide_previndex();
  map.firstChild.coords = '0,0,' + (j >= 0 ? Math.floor(w/2) + ',' + h : '0,0');
  map.firstChild.nextSibling.coords =
    (i >= 0 ? (Math.floor(w/2)+1) + ',0' : w + ',' + h) + ',' + w + ',' + h;
  pa.style.left = sw + 'px';
  na.style.right = sw + 'px';
}

//Class slide
var slide_on=0, // Slideshow is running
    slide_inprog=0, // Slideshow in progress (may be paused)
    slide_timer, // Timer to load next slide
    slide_delay = 5000, // Milliseconds between slides
    slide_order = 1, // Direction: 1=forward, -1=reverse, 0=random
    slide_randomorder = new Array(data_count); // Random index order
function slide_reset() { clearTimeout(slide_timer); }
function slide_nextindex(x) {
  if (slide_on && !slide_order) {
    for (var i=0, j=0; i < data_count; i++)
      if (slide_randomorder[i] == image_index) { j = i + (x?-1:1); break; }
    return (j >= 0 && j < data_count) ? slide_randomorder[j] : -1;
  }
  var j = image_index + (x?-1:1) * (slide_on?slide_order:1);
  return (j >= 0 && j < data_count) ? j : -1;
}
function slide_previndex() {
  return slide_nextindex(1);
}
function slide_next() {
  var i = slide_nextindex(); if (i >= 0) image_show(i); else slide_onoff();
}
function slide_setorder(o) {
  slide_order = parseInt(o);
  slide_setbuttons();
  if (!slide_on) slide_inprog = 0;
}
function slide_setdelay(d) {
  slide_delay = parseInt(d) * 1000;
  if (slide_on) { slide_reset(); slide_go(slide_nextindex()); }
}
function slide_fillrandom(lockfirst) {
  var i, j, k;
  for (i = 0; i < data_count; i++) slide_randomorder[i] = i;
  if (lockfirst=lockfirst?1:0) {
    slide_randomorder[0] = image_index;
    slide_randomorder[image_index] = 0;
  }
  for (i = data_count-1; i > lockfirst; i--) {
    j = lockfirst ? random_int(i)+1 : random_int(i+1);
    k = slide_randomorder[i];
    slide_randomorder[i] = slide_randomorder[j];
    slide_randomorder[j] = k;
  }
}
function slide_setbuttons() {
  ui_vis('slide_poz', slide_on, 1);
  ui_vis('slide_fwd', (!slide_on && slide_order > 0), 1);
  ui_vis('slide_rev', (!slide_on && slide_order < 0), 1);
  ui_vis('slide_rand', (!slide_on && !slide_order), 1);
}
function slide_go(i) {
  if (slide_on) {
    if (i >= 0) slide_timer = setTimeout('slide_next()', slide_delay);
    else { slide_inprog = 0; slide_onoff(); }
  }
}
function slide_onoff() {
  if (!data_count) return;
  slide_on = slide_on?0:1;
  slide_setbuttons();
  if (slide_on) {
    var t = 'slide_next()', d = 1500;
    if (!slide_inprog) {
      if (!slide_order) slide_fillrandom(1);
      if (slide_nextindex() < 0) {
	d = 5;
	if (slide_order > 0) t = 'image_show(0)';
	else if (slide_order < 0) t = 'image_show(' + (data_count-1) + ')';
	else t = 'image_show(' + slide_randomorder[0] + ')';
      }
    }
    slide_inprog = 1;
    slide_timer = setTimeout(t, d);
  } else slide_reset();
  image_setbuttons();
}

//Class thumbs :: div thumbs
var thumbs_horiz=1; // Thumbnail display is horizontal or vertical
function thumbs_horizvert() {
  var main = document.getElementById('gallery');
  if (thumbs_horiz=thumbs_horiz?0:1) {
    image_area.style.left = title_div.style.left = '0';
    thumb_div.className = thumb_div.className.substring(0, thumb_div.className.length-4) + 'Horiz';
    title_div.appendChild(thumb_div);
    if (app_is_ie || app_is_opera) {
      thumb_div.style.width = '100%';
      thumb_div.style.height = 'auto';
    }
    if (app_is_ie) image_area.style.width = title_div.style.width = '100%';
    else if (app_is_safari) document.getElementById('tools_right').style.paddingRight = '0';
  } else {
    main.appendChild(thumb_div);
    thumb_div.className = thumb_div.className.substring(0, thumb_div.className.length-5) + 'Vert';
    if (app_is_ie) {
      var i = 30, o;
      for (o = thumb_div.firstChild; o; o = o.nextSibling)
	if (o.offsetWidth && o.offsetWidth > i) i = o.offsetWidth;
      thumb_div.style.width = (i + 16) + 'px';
      thumb_div.style.height = app_wh + 'px';
      image_area.style.width = title_div.style.width = (app_ww - thumb_div.offsetWidth) + 'px';
    }
    else if (app_is_opera) thumb_div.style.width = (thumb_div.offsetWidth + 16) + 'px';
    else if (app_is_safari) document.getElementById('tools_right').style.paddingRight = '8px';
    image_area.style.left = title_div.style.left = thumb_div.offsetWidth + 'px';
  }
  image_setsize();
  image_fit();
}

//Class options :: div options(div photoActions(select itemLinks))
var options_on=0; // Sidebar/options are visible
function options_setsize() {
  document.getElementById('options').style.bottom = (title_div.offsetHeight + 4) + 'px';
  if (app_is_ie) document.getElementById('options').style.height =
                 (image_area.offsetHeight - 4) + 'px';
}
function options_onoff() {
  if (!options_on) {
    options_setsize();
    if (data_count > 0) options_setItemLinks(image_index);
  }
  ui_vis('options', options_on=options_on?0:1);
}
function options_setItemLinks(idx) {
  var list = document.getElementById('linkList'),
      links = document.getElementById('links_' + idx), i;
  while (list.options.length > 1) { list.options[1] = null; }
  for (i = 0; i < links.options.length; i++) {
    list.options[i+1] = new Option(links.options[i].text, links.options[i].value);
  }
  list.options[0].selected = 1;
  ui_vis('photoActions', links.options.length > 0);
}

//UI Util
function ui_vis(id,vis,inline) {
  var obj = document.getElementById(id);
  obj.style.display = vis ? (inline ? 'inline' : 'block') : 'none';
}
function ui_sethtml(id,html) {
  document.getElementById(id).innerHTML = html;
}
function ui_select(id,val) {
  var obj = document.getElementById(id), i;
  for (i=0; i < obj.options.length; i++) {
    if (obj.options[i].value == val) {
      obj.options[i].selected = 1; break;
    }
  }
}

//Util
function max(a,b) { return (a>b)?a:b; }
function min(a,b) { return (a<b)?a:b; }
function random_int(i) { return Math.floor(i * (Math.random() % 1)); }
function getcookie(k) {
  var i = document.cookie.indexOf(k+'=');
  if (i < 0) return;
  i += k.length + 1;
  var e = document.cookie.indexOf(';', i);
  if (e < 0) e = document.cookie.length;
  return unescape(document.cookie.substring(i, e));
}
