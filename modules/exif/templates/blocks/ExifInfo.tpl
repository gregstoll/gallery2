{*
 * $Revision: 17380 $
 * Read this before changing templates!  http://codex.gallery2.org/Gallery2:Editing_Templates
 *}
{if empty($item)} {assign var=item value=$theme.item} {/if}

{* Load up the EXIF data *}
{g->callback type="exif.LoadExifInfo" itemId=$item.id}

{if !empty($block.exif.LoadExifInfo.exifData)}
  {assign var="exif" value=$block.exif.LoadExifInfo}
  {if empty($ajax)}
    {if $exif.blockNum == 1}
    <script type="text/javascript">
      // <![CDATA[
      function exifSwitchDetailMode(num, itemId, mode) {ldelim}
	url = '{g->url arg1="view=exif.SwitchDetailMode" arg2="itemId=__ITEMID__"
		arg3="mode=__MODE__" arg4="blockNum=__NUM__" htmlEntities=false forceDirect=true}';
	document.getElementById('ExifInfoLabel' + num).innerHTML =
	  '{g->text text="Loading.." forJavascript=true}';
	{literal}
	YAHOO.util.Connect.asyncRequest('GET',
	  url.replace('__ITEMID__', itemId).replace('__MODE__', mode).replace('__NUM__', num),
	  {success: handleExifResponse, failure: handleExifFail, argument: num}, null);
	return false;
      }
      function handleExifResponse(http) {
	document.getElementById('ExifInfoBlock' + http.argument).innerHTML = http.responseText;
      }
      function handleExifFail(http) {
	document.getElementById('ExifInfoLabel' + http.argument).innerHTML = '';
      }
      // ]]>
    </script>{/literal}
    {/if}
    <div id="ExifInfoBlock{$exif.blockNum}" class="{$class}">
  {/if}

  <h3> {g->text text="Photo Properties"} </h3>

  {if isset($exif.mode)}
  <div>{strip}
    {if ($exif.mode == 'summary')}
      {g->text text="summary"}
    {else}
      <a href="{g->url arg1="controller=exif.SwitchDetailMode"
		arg2="mode=summary" arg3="return=true"}" onclick="return exifSwitchDetailMode({$exif.blockNum},{$item.id},'summary')">
	{g->text text="summary"}
      </a>
    {/if}
    &nbsp;&nbsp;
    {if ($exif.mode == 'detailed')}
      {g->text text="details"}
    {else}
      <a href="{g->url arg1="controller=exif.SwitchDetailMode"
		arg2="mode=detailed" arg3="return=true"}" onclick="return exifSwitchDetailMode({$exif.blockNum},{$item.id},'detailed')">
	{g->text text="details"}
      </a>
    {/if}

    <span id="ExifInfoLabel{$exif.blockNum}" style="padding-left:1.5em"></span>
  {/strip}</div>
  {/if}

  {if !empty($exif.exifData)}
  <table class="gbDataTable">
    {section name=outer loop=$exif.exifData step=2}
    <tr>
      {section name=inner loop=$exif.exifData start=$smarty.section.outer.index max=2}
      <td class="gbEven">
	{g->text text=$exif.exifData[inner].title}
      </td>
      <td class="gbOdd">
	{$exif.exifData[inner].value}
      </td>
      {/section}
    </tr>
    {/section}
  </table>
  {/if}
  {if empty($ajax)}</div>{/if}
{/if}
