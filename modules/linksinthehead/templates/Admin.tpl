{*
 * If you want to customize this file, do not edit it directly since future upgrades
 * may overwrite it.  Instead, copy it into a new directory called "local" and edit that
 * version.  Gallery will look for that file first and use it if it exists.
 *}


<div class="gbBlock gcBackground1">
  <h2 class="gbTitle"> {g->text text="Links In The Head - Settings"} </h2>
</div>

  {if !empty($status)}
  <div id="gsStatus">
    {if isset($status.saved)}
    <div class="giStatus">
<br>
      {g->text text="Settings saved successfully"}
<br>
    </div>
    {/if}
  </div>
  {/if}

  <div class="gbAdmin">
    <div class="giDescription">
<br>
      {g->text text="Enter the links you would like displayed in the header. Any URL left empty will not be used."}
    </div>
<br>
    <div class="gbDataEntry">


      <h3 class="giTitle">
	{g->text text="Links"}
      </h3>
        <div class="row" cellspacing="5">
	    {section name=x loop=$form.url}
	        <div class="form-group">
		    <label for="idLink_{$smarty.section.x.index}">{g->text text="Link Name:"}</label>
		    <input id="idLink_{$smarty.section.x.index}" class="form-control" type="text"
		           name="{g->formVar var="form[linkname][`$smarty.section.x.index`]"}" size="12"
		           value="{if isset($form.linkname[x])}{$form.linkname[x]}{/if}"/>
		    <label for="idUrl_{$smarty.section.x.index}">{g->text text="URL:"}</label>
		    <input id="idUrl_{$smarty.section.x.index}" class="form-control" type="text"
		           name="{g->formVar var="form[url][`$smarty.section.x.index`]"}" size="40"
		           value="{$form.url[x]}"/>
	        </div>
	    {/section}
        </div>
    </div>
  </div>

<div class="gbBlock gcBackground1">
    <input type="submit" name="{g->formVar var="form[action][save]"}"
     value="{g->text text="Save"}"/>
  </div>

