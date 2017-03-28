{*
 * $Revision: 16235 $
 * Read this before changing templates!  http://codex.gallery2.org/Gallery2:Editing_Templates
 *}
{g->callback type="customfield.LoadCustomFields" itemId=$item.id|default:$theme.item.id}

{if !empty($block.customfield.LoadCustomFields.fields)}
<div class="{$class}">
  <h3> {g->text text="Custom Fields"} </h3>
  <p class="giDescription">
    {foreach from=$block.customfield.LoadCustomFields.fields key=field item=value}
      {$field}: {$value|markup}<br/>
    {/foreach}
  </p>
</div>
{/if}
