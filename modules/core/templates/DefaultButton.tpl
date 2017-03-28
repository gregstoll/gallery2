{*
 * $Revision: 16877 $
 * Read this before changing templates!  http://codex.gallery2.org/Gallery2:Editing_Templates
 *}
{*
 * Hidden submit button.. to invoke a particular submit button if enter is pressed in a text
 * input it must be first in the form (onkeypress handler to call btn.click() works for some
 * browsers, but not on IE); this button also cannot have display:none or visibility:hidden
 * or IE won't use it.  We need to set display:none for opera because it doesn't allow styling
 * button borders.  IE also needs more than one text field in the form or it won't pass the
 * name/value for any submit button when enter is pressed.. we add an extra field below.
 * !important on background-color below is for IE7 so !important in colorpack doesn't override.
 *}
{assign var="buttonId" value="defaultSubmitBtn`$callCount`"}
<input type="submit" name="{g->formVar var=$name}" value="" id="{$buttonId}" tabindex="-1"
 style="background-color: transparent !important; border-style:none; position:absolute; width:0; right:0"/>
<script type="text/javascript">
  // <![CDATA[
  var a = navigator.userAgent.toLowerCase(), b = document.getElementById('{$buttonId}');
  if (a.indexOf('msie') < 0 || a.indexOf('opera') >= 0) b.style.display = 'none';
  // ]]>
</script>
<input type="text" name="stupidIE" value="" style="display: none"/>
