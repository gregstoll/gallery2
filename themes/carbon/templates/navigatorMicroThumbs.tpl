{*
 * $Revision: 16235 $
 * Read this before changing templates!  http://codex.gallery2.org/Gallery2:Editing_Templates
 *}
{g->callback type="core.LoadPeers" item=$item|default:$theme.item
	     windowSize=$windowSize|default:$theme.params.maxMicroThumbs
	     addEnds=false loadThumbnails=true}

{assign var="data" value=$block.core.LoadPeers}

{if !empty($data.peers)}
<div>
  {assign var="lastIndex" value=0}
  {assign var="columnIndex" value=0}
  <table cellpadding="0" cellspacing="2">
    <tr>
    {foreach from=$data.peers item=peer}
      {assign var="title" value=$peer.title|default:$peer.pathComponent}
      {if ($columnIndex == 4)}
	</tr>
	<tr>
	{assign var="columnIndex" value=0}
      {/if}

      {if (!$peer.canContainChildren && $peer.entityType != 'GalleryLinkItem')}
	{if ($peer.peerIndex == $data.thisPeerIndex)}
	  <td id="microThumbCurrent" align="center" width="44" height="40">
	    {if isset($peer.thumbnail)}
	      {g->image item=$peer image=$peer.thumbnail maxSize=40 title="$title"}
	    {else}
	      {g->text text="no thumbnail"}
	    {/if}
	  </td>
	{else}
	  <td align="center" width="44" height="40">
	    {strip}
	    <a href="{g->url params=$theme.pageUrl arg1="itemId=`$peer.id`"}">
	      {if isset($peer.thumbnail)}
	        {g->image item=$peer image=$peer.thumbnail maxSize=40 title="$title"}
	      {else}
	        {g->text text="no thumbnail"}
	      {/if}
	    </a>
	    {/strip}
	  </td>
	{/if}
	
	{assign var="lastIndex" value=$peer.peerIndex}
	{assign var="columnIndex" value=$columnIndex+1}
      {/if}
    {/foreach}
    </tr>
  </table>
</div>
{/if}
