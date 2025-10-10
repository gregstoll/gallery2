<?xml version="1.0" encoding="utf-8"?>

<!DOCTYPE table
  SYSTEM "../../../../../../../lib/tools/dtd/DatabaseTableDefinition2.0.dtd">
<table>
   <table-name>{$schema.name}</table-name>
   <schema>
      <schema-major>{$schema.major}</schema-major>
      <schema-minor>{$schema.minor}</schema-minor>
   </schema>
{if $requiresId}
   <column>
      <column-name>id</column-name>
      <column-type>INTEGER</column-type>
      <column-size>MEDIUM</column-size>
      <not-null/>
   </column>
{/if}
{foreach from=$members item=member}
   <column>
      <column-name>{$member.name}</column-name>
      <column-type>{$member.type}</column-type>
      <column-size>{$member.size}</column-size>
{if isset($member.required) || isset($member.primary)}
{if !empty($member.required.empty)}
      <not-null empty="{$member.required.empty}"/>
{else}
      <not-null/>
{/if}
{/if}
{if isset($member.default)}
      <default>{$member.default}</default>
{/if}
   </column>
{/foreach}
{foreach from=$keys item=key}
{if !empty($key.primary)}
   <key primary="true">
{else}
   <key>
{/if}
{foreach from=$key.columns item=column}
     <column-name>{$column}</column-name>
{/foreach}
   </key>
{/foreach}
{foreach from=$indexes item=index}
   <index>
{foreach from=$index.columns item=column}
     <column-name>{$column}</column-name>
{/foreach}
   </index>
{/foreach}
</table>
