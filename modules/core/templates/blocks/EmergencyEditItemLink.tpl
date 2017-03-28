{*
 * $Revision: 16235 $
 * Read this before changing templates!  http://codex.gallery2.org/Gallery2:Editing_Templates
 *}
{if !isset($item)} {assign var="item" value=$theme.item} {/if}
{g->callback type="core.ShouldShowEmergencyEditItemLink"
	     permissions=$permissions|default:$theme.permissions
	     checkBlocks=$checkBlocks|default:null
	     checkSidebarBlocks=$checkSidebarBlocks|default:false
	     checkAlbumBlocks=$checkAlbumBlocks|default:false
	     checkPhotoBlocks=$checkPhotoBlocks|default:false}
  {* Use parameter like checkBlocks="sidebar,album" (other check*Blocks params are deprecated) *}

{if ($block.core.ShouldShowEmergencyEditItemLink)}
<div class="{$class}">
  <a href="{g->url arg1="view=core.ItemAdmin" arg2="subView=core.ItemEdit"
		   arg3="itemId=`$item.id`" arg4="return=true"}"> {g->text text="Edit"} </a>
</div>
{/if}
