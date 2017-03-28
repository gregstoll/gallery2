{*
 * $Revision: 17650 $
 * Read this before changing templates!  http://codex.gallery2.org/Gallery2:Editing_Templates
 *}
<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Strict//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-strict.dtd">
<html>
<head>
  <meta http-equiv="Content-Type" content="text/html; charset=UTF-8"/>
  <title>digibug</title>
  {* Only submit the form once.. *}
  <script type="text/javascript">{literal}
    // <![CDATA[
    function go() {
      if (document.cookie.indexOf('G2_digibug=') >= 0) {
	var d = new Date();
	d.setTime(d.getTime() - 10000);
	document.cookie = 'G2_digibug=0;expires=' + d.toUTCString();
	document.getElementById('digibugForm').submit();
      } else {
	history.back();
      }
    }
   // ]]>
  {/literal}</script>
</head>
<body onload="go()">
<form action="http://www.digibug.com/dapi/order.php" method="post" id="digibugForm">
  <input type="hidden" name="digibug_api_version" value="100"/>
  <input type="hidden" name="company_id" value="{$DigibugPrintPhotos.params.digibugCustomerId}"/>
  <input type="hidden" name="event_id" value="{$DigibugPrintPhotos.params.digibugPricelistId}"/>
  <input type="hidden" name="cmd" value="addimg"/>
  <input type="hidden" name="partner_code" value="69"/>
  <input type="hidden" name="return_url" value="{$DigibugPrintPhotos.returnUrl}"/>
  <input type="hidden" name="num_images" value="{$DigibugPrintPhotos.count}"/>
  {foreach from=$DigibugPrintPhotos.entries key=index item=entry}
    <input type="hidden" name="image_{$index}" value="{$entry.imageUrl}"/>
    <input type="hidden" name="image_width_{$index}" value="{$entry.imageWidth}"/>
    <input type="hidden" name="image_height_{$index}" value="{$entry.imageHeight}"/>
    {if isset($entry.thumbUrl)}
      <input type="hidden" name="thumb_{$index}" value="{$entry.thumbUrl}"/>
      <input type="hidden" name="thumb_width_{$index}" value="{$entry.thumbWidth}"/>
      <input type="hidden" name="thumb_height_{$index}" value="{$entry.thumbHeight}"/>
    {/if}
    {if !empty($entry.item.title)}
      <input type="hidden" name="backtext_{$index}" value="{$entry.item.title|markup:none}"/>
    {/if}
  {/foreach}
</form>
<noscript>
  <input type="submit"/>
</noscript>
</body>
</html>
