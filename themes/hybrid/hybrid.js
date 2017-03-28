/****************************
 * Hybrid theme for Gallery2
 * @author Alan Harder <alan.harder@sun.com>
 * $Revision: 20996 $
 */

//Class app
var app_ww, app_wh, // Window width/height
    app_agent = navigator.userAgent.toLowerCase(), // Browser type
    app_version = parseInt(navigator.appVersion),
    app_is_ie = app_agent.indexOf('msie') >= 0 && app_agent.indexOf('opera') < 0,
    app_is_ie7 = app_agent.indexOf('msie 7') >= 0 && app_agent.indexOf('opera') < 0,
    app_is_safari = app_agent.indexOf('safari') >= 0,
    app_is_chrome = app_agent.indexOf('chrome') >= 0,
    app_body; // Scrollable document container (<body> element, or html for IE)
if (window.attachEvent) {
  window.attachEvent("onload", app_onload);
  window.attachEvent("onunload", app_setcookie);
} else if (window.addEventListener) {
  window.addEventListener("load", app_onload, false);
  window.addEventListener("unload", app_setcookie, false);
}
function app_init() {
  if (!app_is_ie && !app_is_safari || app_is_ie7) album_setfixedtitle();
  app_body = document.getElementById('hybridMain');
  while (app_body && app_body.tagName != 'BODY') app_body = app_body.parentNode;
  if (app_is_ie) app_body = app_body.parentNode;
  imagearea = document.getElementById('imagearea');
  imagediv = document.getElementById('imagediv');
  textdiv = document.getElementById('textdiv');

  // Replace <object> (XHTML compliant) with <iframe>
  // Currently <object> works only with Mozilla and Opera.  IE doesn't accept
  // any object.data changes, Safari only accepts new data src when object is
  // visible, Firefox only when object is invisible.  Easier to just use iframe.
  var popup = document.getElementById('popup_details'), iframe = document.createElement('iframe');
  iframe.frameBorder = 0;
  popup.replaceChild(iframe, popup.firstChild);

  if (app_is_chrome) {
    document.onkeydown = app_onkeypress;
  } else {
    document.onkeypress = app_onkeypress;
  }
  if (window.attachEvent) window.attachEvent("onresize", app_onresize);
  else if (window.addEventListener) window.addEventListener("resize", app_onresize, false);

  if (app_is_ie) {
    document.onkeydown = app_onkeydown;
    document.getElementById('imageview').style.position = 'absolute';
    popup.style.position = 'absolute';
    document.getElementById('popup_titlebar').style.position = 'absolute';
  } else if (app_is_safari) {
    document.getElementById('tools_right').style.paddingRight = '8px';
  }

  app_onresize();
  app_getcookie();
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
  if (image_on) { image_setsize(); image_fit(); }
  if (app_is_ie) {
    document.getElementById('popup_details').firstChild.style.height =
      Math.round(app_wh * 0.75) + 'px';
  }
}
function app_onload() {
  if (data_count > 0) image_precache(0);
  if (app_is_safari) album_setfixedtitle();
}
function app_setcookie() {
  var d = new Date(), c = slide_order + ';' + (slide_delay/1000) + ';' + sidebar_on + ';' +
    album_detailson + ';' + album_itemlinkson + ';' + text_on + ';';
  d.setTime(d.getTime() + 90*24*60*60*1000); // 90 day cookie
  document.cookie = 'G2_hybrid=' + escape(c) + ';path=' + cookie_path + ';expires=' + d.toUTCString();
}
function app_getcookie() {
  var c = getcookie('G2_hybrid'), i,j,v,n,it=1;
  if (c) {
    for (i=0, j = c.indexOf(';', 0); j >= 0; i = j+1, j = c.indexOf(';', i)) {
      v = c.substring(i,j);
      n = parseInt(v);
      switch (it++) {
	case 1: ui_select('slide_order', n); slide_setorder(n); break;
	case 2: ui_select('slide_delay', n); slide_setdelay(n); break;
	case 3: if (n) sidebar_onoff(); break;
	case 4: if (!n) album_detailsonoff(); break;
	case 5: if (n) album_itemlinksonoff(); break;
	case 6: if (!n) text_onoff(); break;
      }
    }
  }
  c = getcookie('G2_hybrid_view');
  if (c) {
    document.cookie = 'G2_hybrid_view=;expires=' + new Date().toUTCString();
    if (c == 2 /*start slideshow*/) slide_onoff(slide_order < 0 ? data_view == 0 : data_view > 0);
    else if (data_view >= 0) image_show(data_view);
  }
}
function app_onkeypress(event) {
  if (app_is_ie) event = window.event; //For IE
  var keyCode = event.keyCode ? event.keyCode : event.which;
  if (event.altKey || event.metaKey) return;
  if (app_is_safari) switch (keyCode) { //For Safari
    case 63232: keyCode=38; break;  case 63273: keyCode=36; break;
    case 63233: keyCode=40; break;  case 63275: keyCode=35; break;
    case 63234: keyCode=37; break;  case 63276: keyCode=33; break;
    case 63235: keyCode=39; break;  case 63277: keyCode=34; break;
  }
  /* Album view: space = start slideshow
   *             ctrl-right/left = show/hide sidebar
   *             ctrl-up/down = show/hide item links
   * Image view: space = start/pause slideshow
   *             escape = return to album view
   *             left/right = next/prev image
   *             up/down = show hide description text
   *             page up/down = scroll description text
   *             ctrl-up/down = select full/fit size
   *             arrow keys scroll image in full size; use shift-arrows for funcs above
   * Item Details showing: escape = close popup
   */
  if (keyCode==32) slide_onoff();
  else if (keyCode==27) { if (popup_on) popup_vis(0); else if (image_on) image_vis(0); }
  if (keyCode < 33 || keyCode > 40) return;
  if (event.shiftKey) keyCode += 100;
  if (event.ctrlKey) keyCode += 200;
  if (!image_on) switch (keyCode) {
    case 239: //Ctrl-Right
      if (!sidebar_on) sidebar_onoff();
      break;
    case 237: //Ctrl-Left
      if (sidebar_on) sidebar_onoff();
      break;
    case 238: //Ctrl-Up
      if (!album_itemlinkson) album_itemlinksonoff();
      break;
    case 240: //Ctrl-Down
      if (album_itemlinkson) album_itemlinksonoff();
      break;
  }
  else switch (keyCode) {
    case 37: //Left
      if (image_zoomon) { imagearea.scrollLeft -= 20; break; }
    case 137: //Shift-Left
      image_prev();
      break;
    case 39: //Right
      if (image_zoomon) { imagearea.scrollLeft += 20; break; }
    case 139: //Shift-Right
      image_next();
      break;
    case 38: //Up
      if (image_zoomon) { imagearea.scrollTop -= 20; break; }
    case 138: //Shift-Up
      if (!text_on) text_onoff();
      break;
    case 40: //Down
      if (image_zoomon) { imagearea.scrollTop += 20; break; }
    case 140: //Shift-Down
      if (text_on) text_onoff();
      break;
    case 33: //PageUp
    case 34: //PageDown
      if (text_on) {
	var obj = document.getElementById('text');
	obj.scrollTop += (keyCode==34 ? 1 : -1) * (obj.clientHeight - 5);
      }
      break;
    case 238: //Ctrl-Up
      if (document.getElementById('full_size').style.display == 'inline') image_zoom(1);
      break;
    case 240: //Ctrl-Down
      if (document.getElementById('fit_size').style.display == 'inline') image_zoom(0);
      break;
  }
}
function app_onkeydown() {
  if (window.event.keyCode >= 27 && window.event.keyCode <= 40) {
    app_onkeypress(); window.event.returnValue = false;
  }
}

//Class album :: gsContent(album_titlebar(album_tools,album_desc,album_info),gsAlbumContent)
var album_detailson=1, // Details are visible
    album_itemlinkson=0, // Item links are visible
    album_fixedtitle=0; // Using fixed position for album_titlebar
function album_detailsonoff() {
  ui_vis('album_info', (album_detailson = album_detailson?0:1));
  ui_vis('album_desc', album_detailson);
  ui_sethtml('dtl_link', album_detailson ? album_hidetext : album_showtext);
  if (album_fixedtitle) album_setmargin();
}
function album_itemlinksonoff() {
  var imgs = document.getElementById('gsAlbumContent').getElementsByTagName('IMG');
  album_itemlinkson = album_itemlinkson?0:1;
  for (var i = 0; i < imgs.length; i++)
    if (imgs[i].className == 'popup_button')
      imgs[i].style.display = album_itemlinkson ? 'inline' : 'none';
  ui_sethtml('lnk_link', album_itemlinkson ? album_hidelinks : album_showlinks);
}
function album_setfixedtitle() {
  var t = document.getElementById('album_titlebar');
  if (t.offsetTop == 0) {
    album_fixedtitle = 1;
    album_setmargin();
  } else { // Disable fixed position titlebar if it isn't at very top
    t.style.position = 'static';
  }
}
function album_setmargin() {
  document.getElementById('gsAlbumContent').style.marginTop =
    document.getElementById('album_titlebar').offsetHeight + 'px';
}

//Class sidebar :: div sidebar
var sidebar_on=0; // Sidebar is visible
function sidebar_onoff() {
  ui_vis('sidebar_max', sidebar_on, 1);
  ui_vis('sidebar_min', (sidebar_on = sidebar_on?0:1), 1);
  ui_vis('sidebar', sidebar_on);
  if (app_is_safari) {
    document.getElementById('sidebar').parentNode.style.width = sidebar_on ? 'auto' : '0';
  }
}

//Class image :: div imageview(imagearea(imagediv),textdiv(tools_left,tools_right,title,text))
var image_on=0, // Image is visible
    image_index=0, // Index of visible image
    image_zoomon=0, // Image is zoomed to full size
    image_cache = new Image, // For precaching an image
    image_iscached = new Array(data_count), // Track precached images
    imagearea, imagediv, textdiv, // Containers
    text_on=1, // Description text is visible
    text_empty=1; // Description text is empty
function image_setsize() {
  imagearea.style.height = (app_wh - textdiv.offsetHeight) + 'px';
}
function image_vis(on) {
  if (on) { app_body.saveScrollTop = app_body.parentNode.scrollTop; } //For gecko
  app_body.style.overflow = on ? 'hidden' : 'auto';
  if (!on) { app_body.parentNode.scrollTop = app_body.saveScrollTop; } //For gecko
  app_getwinsize();
  if (!on && slide_on) slide_onoff();
  if (app_is_ie && sidebar_on) ui_vis('sidebar', on?0:1); //For IE (hide <select>s)
  ui_vis('imageview', image_on=on);
  if (!on) {
    if (!app_is_safari) imagearea.scrollLeft = imagearea.scrollTop = 0; //avoid Safari crash
    imagediv.innerHTML = '';
  } else if (app_is_ie) {
    var i = document.getElementById('imageview');
    i.style.top = app_body.scrollTop + 'px';
    i.style.left = app_body.scrollLeft + 'px';
  }
}
function image_show(i) {
  if (!image_on) image_vis(1);
  slide_reset();
  image_index = i;
  ui_sethtml('title', document.getElementById('title_'+image_index).innerHTML);
  ui_sethtml('date', document.getElementById('date_'+image_index).innerHTML);
  image_setsize();
  if (data_iw[i] < 0) {
    imagediv.innerHTML = '<iframe style="width:100%;height:' + (imagearea.offsetHeight - 4)
      + 'px" frameborder="0" src="' + document.getElementById('img_'+i).href + '"></iframe>';
    ui_vis('fit_size', 0); ui_vis('full_size', 0);
  } else {
    var s = image_fit(1);
    imagediv.innerHTML = '<img name="view" src="' + document.getElementById('img_'+i).href
      + '" ' + s + ' onload="image_loaded()" alt=""/>';
  }
  image_setbuttons();
  text_fill();
  if (slide_inprog && !slide_on) slide_inprog = 0;
}
function image_fit(getstr) {
  if (!getstr && !document.view) return;
  var w = data_iw[image_index], h = data_ih[image_index],
      aw = app_ww, ah = imagearea.offsetHeight, a=0;
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
  imagediv.style.paddingTop = Math.floor((ah-h)/2) + 'px';
  ui_vis('fit_size', 0);
  ui_vis('full_size', a, 1);
  if (getstr) {
    return 'width="' + w + '" height="' + h + '"';
  } else {
    document.view.width = w;
    document.view.height = h;
  }
}
function image_zoom(on,noresize) {
  imagearea.style.overflow = on ? 'auto' : 'hidden';
  if (!on) imagearea.scrollLeft = imagearea.scrollTop = 0;
  if (image_zoomon=on) {
    ui_vis('full_size', 0);
    ui_vis('fit_size', 1, 1);
    var h = imagearea.offsetHeight - data_ih[image_index];
    imagediv.style.paddingTop = max(Math.floor(h/2), 0) + 'px';
    document.view.width = data_iw[image_index];
    document.view.height = data_ih[image_index];
  } else if (!noresize) { image_fit(); }
}
function image_precache(i) {
  if (!image_iscached[i]) {
    image_iscached[i] = 1;
    image_cache.src = document.getElementById('img_'+i).href;
  }
}
function image_loaded() {
  var i = slide_nextindex(), j = i; if (j < 0 || image_iscached[j]) j = slide_previndex();
  if (j >= 0) image_precache(j);
  slide_go(i);
}
function image_next() {
  var i = slide_nextindex(); if (i >= 0) image_show(i);
  else { var go = (slide_on && slide_order < 0) ? prev_page : next_page;
	 if (go) location.href = go.replace('__SLIDE__', slide_on); else return -1; }
}
function image_prev() {
  var i = slide_previndex(); if (i >= 0) image_show(i);
  else { var go = (slide_on && slide_order < 0) ? next_page : prev_page;
	 if (go) location.href = go.replace('__SLIDE__', slide_on); }
}
function image_setbuttons() {
  var i = slide_nextindex(), j = slide_previndex(),
      has_next_page = (slide_on && slide_order < 0) ? prev_page : next_page,
      has_prev_page = (slide_on && slide_order < 0) ? next_page : prev_page;
  ui_vis('next_img', i >= 0, 1);
  ui_vis('next_off', i < 0 && !has_next_page, 1);
  ui_vis('next_page', i < 0 && has_next_page, 1);
  ui_vis('prev_img', j >= 0, 1);
  ui_vis('prev_off', j < 0 && !has_prev_page, 1);
  ui_vis('prev_page', j < 0 && has_prev_page, 1);
  var last_empty = text_empty;
  text_empty = document.getElementById('text_'+image_index).innerHTML ?0:1;
  if (!text_on) {
    ui_vis('text_on', !text_empty, 1);
    ui_vis('text_none', text_empty, 1);
  }
  else if (last_empty != text_empty) {
    ui_vis('text', !text_empty);
    if (image_on) { image_setsize(); image_fit(); }
  }
}
function text_onoff() {
  if ((text_on = text_on?0:1) && data_count > 0) text_fill();
  ui_vis(text_empty ? 'text_none' : 'text_on', !text_on, 1);
  ui_vis('text_off', text_on, 1);
  ui_vis('text', text_on && !text_empty);
  if (image_on) { image_setsize(); image_fit(); }
}
function text_fill() {
  if (text_on) ui_sethtml('text', document.getElementById('text_'+image_index).innerHTML);
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
  if (image_next() < 0) { slide_inprog = 0; slide_onoff(); }
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
  if (!data_count) return;
  ui_vis('slide_poz', slide_on, 1);
  ui_vis('slide_fwd', (!slide_on && slide_order > 0), 1);
  ui_vis('slide__fwd', (slide_order > 0), 1);
  ui_vis('slide_rev', (!slide_on && slide_order < 0), 1);
  ui_vis('slide__rev', (slide_order < 0), 1);
  ui_vis('slide_rand', (!slide_on && !slide_order), 1);
  ui_vis('slide__rand', !slide_order, 1);
}
function slide_go(i) {
  if (slide_on) {
    if (i >= 0 || (slide_order < 0 ? prev_page : next_page))
      slide_timer = setTimeout('slide_next()', slide_delay);
    else { slide_inprog = 0; slide_onoff(); }
  }
}
function slide_onoff(start_last) {
  if (!data_count) return;
  slide_on = slide_on?0:1;
  slide_setbuttons();
  if (slide_on) {
    var t = 'slide_next()', d = 1500;
    if (slide_inprog) {
      if (!image_on) image_show(image_index);
    } else {
      if (!slide_order) slide_fillrandom(image_on);
      if (!image_on || slide_nextindex() < 0) {
	d = 5;
	t = 'image_show(';
	if (slide_order) t += (slide_order + (start_last?2:0) == 1 ? 0 : data_count - 1) + ')';
	else t += (start_last ? slide_randomorder[data_count-1] : slide_randomorder[0]) + ')';
      }
    }
    slide_inprog = 1;
    slide_timer = setTimeout(t, d);
  } else slide_reset();
  image_setbuttons();
}

//Class popup :: div popup_menu :: div popup_details(iframe), div popup_titlebar
var popup_timer, // Timer for hiding popup_menu
    popup_on=0; // Popup iframe is visible
function popup_menu(event,i,ii) {
  clearTimeout(popup_timer);
  var pop = document.getElementById('popup_menu'), obj = document.getElementById('links_' + i);
  pop.style.visibility = 'hidden';
  pop.style.display = 'block';
  var links = ii >= 0 ? '<a href="" onclick="popup_info(' + ii +
                        ');this.blur();return false">' + item_details + '</a><br/>' : '';
  if (obj) links += obj.innerHTML;
  pop.style.left = '0';
  pop.style.width = 'auto';
  ui_sethtml('popup_links', links);
  pop.style.width = pop.offsetWidth + 'px';
  if (!event) event = window.event;
  var pw = pop.offsetWidth, ph = pop.offsetHeight,
      iw = event.target ? event.target.width : event.srcElement.width, //Gecko+Opera : IE
      ix = (event.target && event.target.x) ? event.target.x //Gecko
	 : event.pageX ? (event.pageX - event.offsetX) //Opera
	 : (event.x - event.offsetX + app_body.scrollLeft - 2), //IE
      iy = (event.target && event.target.y) ? event.target.y //Gecko
	 : event.pageY ? (event.pageY - event.offsetY) //Opera
	 : (event.y - event.offsetY + app_body.scrollTop - 2), //IE
      sy = (typeof(window.scrollY)=='number' ? window.scrollY : app_body.scrollTop); //Gecko:other
  pop.style.left = (ix + iw - pw) + 'px';
  pop.style.top = min(iy, app_wh + sy - ph) + 'px'; // Maybe too low on gecko with horiz scrollbar
  pop.style.visibility = 'visible';
}
function popup_info(i) {
  var o = document.getElementById('popup_details').firstChild,
      href = document.getElementById('info_' + (i >= 0 ? i : image_index)).href;
  o.src = href;  // See app_init; use o.data = href if browsers ever support object.
  popup_vis(1);
}
function popup_vis(on) {
  if (app_is_ie) {
    var d = document.getElementById('popup_details'), t = document.getElementById('popup_titlebar');
    t.style.top = (app_body.scrollTop + Math.round(app_wh * 0.11)) + 'px';
    d.style.top = (app_body.scrollTop + Math.round(app_wh * 0.14)) + 'px';
    t.style.left = d.style.left = (app_body.scrollLeft + Math.round(app_ww * 0.15)) + 'px';
  }
  ui_vis('popup_details', popup_on=on);
  ui_vis('popup_titlebar', on);
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
