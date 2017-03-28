{*
 * $Revision: 16235 $
 * Read this before changing templates!  http://codex.gallery2.org/Gallery2:Editing_Templates
 *}
{if empty($theme.params.sidebarBlocks)}
  {include file="gallery:`$theme.moduleTemplate`" l10Domain=$theme.moduleL10Domain}
{else}
<table width="100%" cellspacing="0" cellpadding="0">
  <tr valign="top">
    <td id="gsSidebarCol">
      {g->theme include="sidebar.tpl"}
    </td>
    <td>
      {include file="gallery:`$theme.moduleTemplate`" l10Domain=$theme.moduleL10Domain}
    </td>
  </tr>
</table>
{/if}
