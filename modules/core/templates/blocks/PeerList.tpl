{*
 * $Revision: 16235 $
 * Read this before changing templates!  http://codex.gallery2.org/Gallery2:Editing_Templates
 *}
{g->callback type="core.LoadPeers" item=$item|default:$theme.item
	     windowSize=$windowSize|default:null}
{assign var="data" value=$block.core.LoadPeers}

{if !empty($data.peers)}
<div class="{$class}">
  <h3 class="parent"> {$data.parent.title|default:$data.parent.pathComponent|markup} </h3>

  {assign var="lastIndex" value=0}
  {foreach from=$data.peers item=peer}
    {assign var="title" value=$peer.title|default:$peer.pathComponent}
    {if ($peer.peerIndex - $lastIndex > 1)}
      <span class="neck">...</span>
    {/if}
    {if ($peer.peerIndex == $data.thisPeerIndex)}
      <span class="current">
	{g->text text="%d. %s" arg1=$peer.peerIndex arg2=$title|markup:strip|entitytruncate:14}
      </span>
    {else}
      <a href="{g->url params=$theme.pageUrl arg1="itemId=`$peer.id`"}">
	{g->text text="%d. %s" arg1=$peer.peerIndex arg2=$title|markup:strip|entitytruncate:14}
      </a>
    {/if}
    {assign var="lastIndex" value=$peer.peerIndex}
  {/foreach}
</div>
{/if}
