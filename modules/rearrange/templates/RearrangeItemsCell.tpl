{*
 * $Revision: 16235 $
 * Read this before changing templates!  http://codex.gallery2.org/Gallery2:Editing_Templates
 *}
{strip}
{if $child.canContainChildren}
  {assign var="riClass" value=riAlbum}
{else}
  {assign var="riClass" value=riItem}
{/if}
{capture assign="riTitle"}{$child.title|markup:strip|default:$child.pathComponent} ({g->date
			   timestamp=$child.originationTimestamp}){/capture}
{if isset($child.thumbnail)}
  {g->image item=$child image=$child.thumbnail maxSize=100 class=$riClass title=$riTitle}
{else}
  <div class="{$riClass}">{$riTitle}</div>
{/if}
{/strip}
