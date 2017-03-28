{*
 * $Revision: 16235 $
 * Read this before changing templates!  http://codex.gallery2.org/Gallery2:Editing_Templates
 *}
{if empty($item)} {assign var=item value=$theme.item} {/if}

{* Load up the Getid3 data *}
{g->callback type="getid3.LoadGetid3Info" itemId=$item.id}

{if !empty($block.getid3.LoadGetid3Info.getid3Data)}
<div class="{$class}">
  <h3> {g->text text="Audio/Video Properties"} </h3>
  {if isset($block.getid3.LoadGetid3Info.mode)}

  <div>
    {if ($block.getid3.LoadGetid3Info.mode == 'summary')}
      {g->text text="summary"}
    {else}
      <a href="{g->url arg1="controller=getid3.Getid3DetailMode" arg2="mode=summary"
       arg3="return=true"}">
        {g->text text="summary"}
      </a>
    {/if}

    {if ($block.getid3.LoadGetid3Info.mode == 'detailed')}
      {g->text text="details"}
    {else}
      <a href="{g->url arg1="controller=getid3.Getid3DetailMode" arg2="mode=detailed"
       arg3="return=true"}">
        {g->text text="details"}
      </a>
    {/if}
  </div>
  {else}
  <div>
     {g->text text="summary"}
     {g->text text="details"}
  </div>
  {/if}

  {if !empty($block.getid3.LoadGetid3Info.getid3Data)}
  <table class="gbDataTable">
    {section name=outer loop=$block.getid3.LoadGetid3Info.getid3Data step=2}
    <tr> <!--class="{cycle values='gbEven,gbOdd'}"-->
      {section name=inner loop=$block.getid3.LoadGetid3Info.getid3Data start=$smarty.section.outer.index max=2}
      <td class="gbEven">
	{g->text text=$block.getid3.LoadGetid3Info.getid3Data[inner].title}
      </td>
      <td class="gbOdd">
	{$block.getid3.LoadGetid3Info.getid3Data[inner].value}
      </td>
      {/section}
    </tr>
    {/section}
  </table>
  {/if}
</div>
{/if}
{if !empty($block.getid3.LoadGetid3Info.getid3ArchiveData)}
<div class="{$class}">
  <h3> {g->text text="Archive File List"} </h3>
  <table class="gbDataTable">
    <tr>
      <td class="gbEven">
        {g->text text="Name"}
      </td><td class="gbOdd">
        {g->text text="Size"}
      </td>
    </tr>
    {foreach name=fileIndex from=$block.getid3.LoadGetid3Info.getid3ArchiveData item=file}
    <tr>
      <td class="gbEven">
        {$file.name}
      </td><td class="gbOdd">
        {$file.size}
      </td><td>
      </td>
    </tr>
    {/foreach}
  </table>
</div>
{/if}
