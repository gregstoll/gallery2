{*
 * $Revision: 17422 $
 * Read this before changing templates!  http://codex.gallery2.org/Gallery2:Editing_Templates
 *}
{g->callback type="imageblock.LoadImageBlock"
	     blocks=$blocks|default:null repeatBlock=$repeatBlock|default:null
	     maxSize=$maxSize|default:null itemId=$itemId|default:null
	     link=$link|default:null linkTarget=$linkTarget|default:null
	     useDefaults=$useDefaults|default:true showHeading=$showHeading|default:true
	     showTitle=$showTitle|default:true showDate=$showDate|default:true
	     showViews=$showViews|default:false showOwner=$showOwner|default:false
	     show=$show|default:null exactSize=$exactSize|default:null
	     itemFrame=$itemFrame|default:null albumFrame=$albumFrame|default:null}

{if !empty($ImageBlockData)}
<div class="{$class}">
  {include file="gallery:modules/imageblock/templates/ImageBlock.tpl"}
</div>
{/if}
