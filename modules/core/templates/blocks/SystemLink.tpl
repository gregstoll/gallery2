{*
 * $Revision: 16235 $
 * Read this before changing templates!  http://codex.gallery2.org/Gallery2:Editing_Templates
 *}
{if isset($theme.systemLinks[$linkId])}
<span class="{$class}">
  <a href="{g->url
     params=$theme.systemLinks[$linkId].params}">{$theme.systemLinks[$linkId].text}</a>
</span>
{/if}
