{*
 * $Revision: 17650 $
 * Read this before changing templates!  http://codex.gallery2.org/Gallery2:Editing_Templates
 *}
<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Strict//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-strict.dtd">
<html>
<head>
  <meta http-equiv="Content-Type" content="text/html; charset=UTF-8"/>
  <title>shutterfly</title>
  {* Only submit the form once.. *}
  <script type="text/javascript">{literal}
    // <![CDATA[
    function go() {
      if (document.cookie.indexOf('G2_shutterfly=') >= 0) {
	var d = new Date();
	d.setTime(d.getTime() - 10000);
	document.cookie = 'G2_shutterfly=0;expires=' + d.toUTCString();
	document.getElementById('shutterflyForm').submit();
      } else {
	history.back();
      }
    }
   // ]]>
  {/literal}</script>
</head>
<body onload="go()">
<form action="http://www.shutterfly.com/c4p/UpdateCart.jsp" method="post" id="shutterflyForm">
  <input type="hidden" name="protocol" value="SFP,100"/>
  <input type="hidden" name="pid" value="C4PP"/>
  <input type="hidden" name="psid" value="GALL"/>
  <input type="hidden" name="referid" value="gallery"/>
  <input type="hidden" name="returl" value="{$PrintPhotos.returnUrl}"/>
  <input type="hidden" name="addim" value="1"/>
  <input type="hidden" name="imnum" value="{$PrintPhotos.count}"/>
  {foreach from=$PrintPhotos.entries key=index item=entry}
    <input type="hidden" name="imraw-{$index}" value="{$entry.imageUrl}"/>
    <input type="hidden" name="imrawwidth-{$index}" value="{$entry.imageWidth}"/>
    <input type="hidden" name="imrawheight-{$index}" value="{$entry.imageHeight}"/>
    {if isset($entry.thumbUrl)}
      <input type="hidden" name="imthumb-{$index}" value="{$entry.thumbUrl}"/>
      <input type="hidden" name="imthumbwidth-{$index}" value="{$entry.thumbWidth}"/>
      <input type="hidden" name="imthumbheight-{$index}" value="{$entry.thumbHeight}"/>
    {/if}
    {if !empty($entry.item.title)}
      <input type="hidden" name="imbkprnta-{$index}" value="{$entry.item.title|markup:strip}"/>
    {/if}
  {/foreach}
</form>
<noscript>
  <input type="submit"/>
</noscript>
</body>
</html>
