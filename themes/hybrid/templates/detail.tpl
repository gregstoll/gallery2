{*
 * $Revision: 16235 $
 * Read this before changing templates!  http://codex.gallery2.org/Gallery2:Editing_Templates
 *}
<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Strict//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-strict.dtd">
<html><head>
<title>
  {$theme.item.title|markup:strip|default:$theme.item.pathComponent}
</title>
{g->head}
</head>
<body class="gallery">
<div style="padding: 0.7em" {g->mainDivAttributes}>
  <table cellspacing="0"><tr>
  {if isset($theme.thumbnail)}
    <td>{g->image item=$theme.item image=$theme.thumbnail class="giThumbnail"}</td>
  {/if}
  <td valign="top" class="giDescription" style="padding: 0.7em 0 0 0.7em">
    <strong>{$theme.item.title|markup}</strong><br/>
    {g->text text="Owner: %s" arg1=$theme.item.owner.fullName|default:$theme.item.owner.userName}
    <br/>
    {g->text one="Viewed: %d time" many="Viewed: %d times"
	     count=$theme.item.viewCount arg1=$theme.item.viewCount}
    <br/>
    {g->text text="Date: "}{g->date timestamp=$theme.item.originationTimestamp}
    <br/>
    {if isset($theme.item.keywords)}
      {g->text text="Keywords: "}{$theme.item.keywords|markup}
      <br/>
    {/if}
    {g->text text="Link to this item:"}
    <a href="{g->url arg1="view=core.ShowItem" arg2="itemId=`$theme.item.id`"
		     forceSessionId=false}">
      {g->url arg1="view=core.ShowItem" arg2="itemId=`$theme.item.id`"
	      forceSessionId=false forceFullUrl=true}
    </a>
  </td></tr></table>
  {* Show any other item blocks *}
  {foreach from=$theme.params.photoBlocks item=block}
    {g->block type=$block.0 params=$block.1 class="gbBlock"}
  {/foreach}
</div>
</body></html>
