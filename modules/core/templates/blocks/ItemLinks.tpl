{*
 * $Revision: 16931 $
 * Read this before changing templates!  http://codex.gallery2.org/Gallery2:Editing_Templates
 *}
{if !isset($links) && isset($theme.itemLinks)}
  {assign var="links" value=$theme.itemLinks}
{/if}
{if !empty($links)}
  {if empty($item)}
    {assign var="item" value=$theme.item}
  {/if}
  {if !isset($lowercase)}
    {assign var="lowercase" value=false}
  {/if}
  {if !isset($useDropdown)}
    {assign var="useDropdown" value=true}
  {/if}

  <div class="{$class}">
    {* If more than one link and $useDropdown is true, use a dropdown.  Otherwise render as links. *}
    {if count($links) > 1 && $useDropdown}
      <select onchange="var value = this.value; this.options[0].selected = true; eval(value)">
	<option value="">
	  {if $item.canContainChildren}
	    {g->text text="&laquo; album actions &raquo;"}
	  {else}
	    {g->text text="&laquo; item actions &raquo;"}
	  {/if}
	</option>
	{foreach from=$links item="link"}
	  {g->itemLink link=$link type="option" lowercase=$lowercase}
	{/foreach}
      </select>
    {else}
      {foreach from=$links item="link"}
	{g->itemLink link=$link lowercase=$lowercase}
      {/foreach}
    {/if}
  </div>
{/if}
