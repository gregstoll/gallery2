{*
 * $Revision: 996 $
 * Read this before changing templates!  http://codex.gallery2.org/Gallery2:Editing_Templates
 *}
{* Set defaults *}
{if empty($item)} {assign var=item value=$theme.item} {/if}
{if empty($width)} {assign var=width value=85} {/if}
{if !isset($showWarnings)} {assign var=showWarnings value=1} {/if}

{g->callback type="geturls.LoadUrls" itemId=$item.id}

{if !empty($block.geturls.LoadUrls)}
<div class="{$class}">
{include file="gallery:modules/geturls/templates/GetUrls.tpl" 
    GetUrlsData=$block.geturls.LoadUrls
    width=$width
    showWarnings=$showWarnings
    isBlock=1}
</div>
{/if}
