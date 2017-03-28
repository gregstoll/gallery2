{*
 * $Revision: 16235 $
 * Read this before changing templates!  http://codex.gallery2.org/Gallery2:Editing_Templates
 *}
{if $callCount == 1}{literal}
<script type="text/javascript">
// <![CDATA[
var dim_timer = new Array();
function dim_keypress(i,w,e) {
  var key = window.event ? window.event.keyCode : e.which;
  var h = document.getElementById(w.id + '_h');
  if (((key >= 48 && key <= 57) || key == 8) && (w.value == h.value)) {
    clearTimeout(dim_timer[i]);
    dim_timer[i] = setTimeout('dim_copy("' + w.id + '")', 500);
  }
}
function dim_keydown(i,w) {
  // IE only gets backspace via keydown..
  if (window.event && window.event.keyCode == 8) dim_keypress(i,w);
}
function dim_copy(id) {
  var w = document.getElementById(id), h = document.getElementById(id + '_h');
  h.value = w.value;
}
// ]]>
</script>
{/literal}{/if}
<input type="text" id="{$formVarId}" size="6" name="{g->formVar var=$formVar}[width]"
 onkeypress="dim_keypress({$callCount},this,event)" onkeydown="dim_keydown({$callCount},this)"
 {if isset($width)}value="{$width}"{/if}/>
x
<input type="text" id="{$formVarId}_h" size="6" name="{g->formVar var=$formVar}[height]"
 {if isset($height)}value="{$height}"{/if}/>
