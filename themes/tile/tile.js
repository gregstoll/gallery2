/****************************
 * Tile theme for Gallery2
 * @author Alan Harder <alan.harder@sun.com>
 * $Revision: 15342 $
 */

if (window.attachEvent) { // IE
  window.attachEvent("onload", app_onload);
} else if (window.addEventListener) {
  window.addEventListener("load", app_onload, false);
}

function app_init() {
  var c = getcookie('G2_tile_view');
  if (c) document.cookie = 'G2_tile_view=;expires=' + new Date().toUTCString();
  if (c && view >= 0) image_show(view);
}
function app_onload() {
  if (image_width.length > 0) image_precache(0);
}

var image_cache = new Image, // For precaching an image
    image_iscached = new Array(image_width.length); // Track precached images
function image_show(i) {
  if (image_width[i] < 0) {
    ui_vis('image', 1);
    var div = document.getElementById('image');
    ui_sethtml('image_view', '<iframe style="width:100%;height:' + (div.offsetHeight - 60)
     + 'px" frameborder="0" src="' + document.getElementById('img_'+i).href + '"></iframe>');
  } else {
    ui_sethtml('image_view', '<img src="' + document.getElementById('img_'+i).href + '" width="'
     + image_width[i] + '" height="' + image_height[i] + '" onload="image_loaded()" alt=""/>');
    ui_vis('image', 1);
  }
  ui_sethtml('title', document.getElementById('title_'+i).innerHTML);
}
function image_loaded() {
  for (var i = 0; i < image_iscached.length; i++)
    if (!image_iscached[i]) { image_precache(i); break; }
}
function image_precache(i) {
  if (!image_iscached[i]) {
    image_iscached[i] = 1;
    image_cache.src = document.getElementById('img_'+i).href;
  }
}

function ui_vis(id,vis) {
  var obj = document.getElementById(id);
  obj.style.display = vis ? 'block' : 'none';
}
function ui_sethtml(id,html) {
  document.getElementById(id).innerHTML = html;
}
function getcookie(k) {
  var i = document.cookie.indexOf(k+'=');
  if (i < 0) return;
  i += k.length+1;
  var e = document.cookie.indexOf(';', i);
  if (e < 0) e = document.cookie.length;
  return unescape(document.cookie.substring(i, e));
}

